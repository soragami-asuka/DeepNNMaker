//======================================
// フィードフォワードニューラルネットワークの統合処理レイヤー
// 結合、活性化
// CPU処理用
//======================================
#include"stdafx.h"

#include"Activation_DATA.hpp"
#include"Activation_FUNC.hpp"
#include"Activation_Base.h"

#include"Activation_GPU.cuh"
#include"Activation_LayerData_GPU.cuh"

using namespace Gravisbell;
using namespace Gravisbell::Layer::NeuralNetwork;

namespace Gravisbell {
namespace Layer {
namespace NeuralNetwork {


	/** コンストラクタ */
	Activation_GPU::Activation_GPU(Gravisbell::GUID guid, Activation_LayerData_GPU& i_layerData)
		:	Activation_Base	(guid)
		,	layerData						(i_layerData)	/**< レイヤーデータ */
		,	inputBufferCount				(0)		/**< 入力バッファ数 */
		,	outputBufferCount				(0)		/**< 出力バッファ数 */
		,	cudnnHandle		(NULL)
		,	activDesc		(NULL)
		,	inputTensorDesc	(NULL)
		,	outputTensorDesc	(NULL)
	{
		cudnnCreate(&cudnnHandle);
		cudnnCreateTensorDescriptor(&inputTensorDesc);
		cudnnCreateTensorDescriptor(&outputTensorDesc);
		cudnnCreateActivationDescriptor(&activDesc);
	}
	/** デストラクタ */
	Activation_GPU::~Activation_GPU()
	{
		if(inputTensorDesc)		cudnnDestroyTensorDescriptor(inputTensorDesc);
		if(outputTensorDesc)	cudnnDestroyTensorDescriptor(outputTensorDesc);
		if(activDesc)			cudnnDestroyActivationDescriptor(activDesc);
		if(cudnnHandle)			cudnnDestroy(cudnnHandle);
	}


	//================================
	// 基本処理
	//================================
	/** レイヤー種別の取得 */
	U32 Activation_GPU::GetLayerKind()const
	{
		return Layer::ELayerKind::LAYER_KIND_CPU | GetLayerKindBase();
	}

	/** 初期化. 各ニューロンの値をランダムに初期化
		@return	成功した場合0 */
	ErrorCode Activation_GPU::Initialize(void)
	{
		return this->layerData.Initialize();
	}


	//===========================
	// レイヤーデータ関連
	//===========================
	/** レイヤーデータを取得する */
	Activation_LayerData_Base& Activation_GPU::GetLayerData()
	{
		return this->layerData;
	}
	const Activation_LayerData_Base& Activation_GPU::GetLayerData()const
	{
		return this->layerData;
	}


	//===========================
	// レイヤー保存
	//===========================
	/** レイヤーをバッファに書き込む.
		@param o_lpBuffer	書き込み先バッファの先頭アドレス. GetUseBufferByteCountの戻り値のバイト数が必要
		@return 成功した場合書き込んだバッファサイズ.失敗した場合は負の値 */
	S32 Activation_GPU::WriteToBuffer(BYTE* o_lpBuffer)const
	{
		return this->layerData.WriteToBuffer(o_lpBuffer);
	}


	//================================
	// 演算処理
	//================================
	/** 演算前処理を実行する.(学習用)
		@param batchSize	同時に演算を行うバッチのサイズ.
		NN作成後、演算処理を実行する前に一度だけ必ず実行すること。データごとに実行する必要はない.
		失敗した場合はPreProcessLearnLoop以降の処理は実行不可. */
	ErrorCode Activation_GPU::PreProcessLearn(unsigned int batchSize)
	{
		ErrorCode errorCode = this->PreProcessCalculate(batchSize);
		if(errorCode != ErrorCode::ERROR_CODE_NONE)
			return errorCode;

		// 入力差分バッファを作成
		switch(this->layerData.layerStructure.ActivationType)
		{
			// lenear
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_lenear:
			break;

		default:
			this->lpDInputBuffer_d.resize(this->batchSize * this->inputBufferCount);
			break;
		}

		return ErrorCode::ERROR_CODE_NONE;
	}


	/** 演算前処理を実行する.(演算用)
		@param batchSize	同時に演算を行うバッチのサイズ.
		NN作成後、演算処理を実行する前に一度だけ必ず実行すること。データごとに実行する必要はない.
		失敗した場合はCalculate以降の処理は実行不可. */
	ErrorCode Activation_GPU::PreProcessCalculate(unsigned int batchSize)
	{
		this->batchSize = batchSize;

		// 入力バッファ数を確認
		this->inputBufferCount = this->GetInputBufferCount();
		if(this->inputBufferCount == 0)
			return ErrorCode::ERROR_CODE_FRAUD_INPUT_COUNT;

		// 出力バッファ数を確認
		this->outputBufferCount = this->GetOutputBufferCount();
		if(this->outputBufferCount == 0)
			return ErrorCode::ERROR_CODE_FRAUD_OUTPUT_COUNT;

		// 出力バッファを作成
		switch(this->layerData.layerStructure.ActivationType)
		{
			// lenear
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_lenear:
			break;

		default:
			this->lpOutputBuffer_d.resize(this->batchSize * this->outputBufferCount);
			{
				int n = this->batchSize;
				int c = this->GetOutputDataStruct().ch;
				int h = this->GetOutputDataStruct().z * this->GetOutputDataStruct().y;
				int w = this->GetOutputDataStruct().x;

				const int nDims = 4;
				int dimA[nDims] = {n, c, h, w};
				int strideA[nDims] = {c*h*w, h*w, w, 1};

				cudnnStatus_t err = cudnnSetTensorNdDescriptor(this->outputTensorDesc,
					CUDNN_DATA_FLOAT,
					4,
					dimA,
					strideA );

				if(err != 0)
					break;

				err = cudnnSetTensorNdDescriptor(this->inputTensorDesc,
					CUDNN_DATA_FLOAT,
					4,
					dimA,
					strideA );

				if(err != 0)
					break;
			}
			break;
		}


		// 活性化関数を設定
		switch(this->layerData.layerStructure.ActivationType)
		{
			// lenear
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_lenear:
			break;

			// Sigmoid
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_sigmoid:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_sigmoid_crossEntropy:
		default:
			cudnnSetActivationDescriptor(activDesc,
										CUDNN_ACTIVATION_SIGMOID,
										CUDNN_PROPAGATE_NAN,
										0.0);
			break;

			// ReLU
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_ReLU:
			cudnnSetActivationDescriptor(activDesc,
										CUDNN_ACTIVATION_RELU,
										CUDNN_PROPAGATE_NAN,
										0.0);
			break;

			// tanh
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_tanh:
			cudnnSetActivationDescriptor(activDesc,
										CUDNN_ACTIVATION_TANH,
										CUDNN_PROPAGATE_NAN,
										0.0);
			break;

			// SoftMax系
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_ALL:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_CH:
			break;
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_ALL_crossEntropy:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_CH_crossEntropy:
			break;
		}

		return ErrorCode::ERROR_CODE_NONE;
	}


	/** 学習ループの初期化処理.データセットの学習開始前に実行する
		失敗した場合はCalculate以降の処理は実行不可. */
	ErrorCode Activation_GPU::PreProcessLearnLoop(const SettingData::Standard::IData& data)
	{
		if(this->pLearnData != NULL)
			delete this->pLearnData;
		this->pLearnData = data.Clone();

		return Gravisbell::ErrorCode::ERROR_CODE_NONE;
	}
	/** 演算ループの初期化処理.データセットの演算開始前に実行する
		失敗した場合はCalculate以降の処理は実行不可. */
	ErrorCode Activation_GPU::PreProcessCalculateLoop()
	{
		return Gravisbell::ErrorCode::ERROR_CODE_NONE;
	}


	/** 演算処理を実行する.
		@param lpInputBuffer	入力データバッファ. GetInputBufferCountで取得した値の要素数が必要
		@return 成功した場合0が返る */
	ErrorCode Activation_GPU::Calculate(CONST_BATCH_BUFFER_POINTER i_lpInputBuffer)
	{
		// 入力バッファのアドレスを配列に格納
		this->m_lpInputBuffer_d = i_lpInputBuffer;


		switch(this->layerData.layerStructure.ActivationType)
		{
			// lenear
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_lenear:
			break;

		default:
			// Sigmoid
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_sigmoid:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_sigmoid_crossEntropy:
			// ReLU
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_ReLU:
			// tanh
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_tanh:
			{
				F32 alpha = 1.0f;
				F32 beta = 0.0f;
				cudnnActivationForward(
					this->cudnnHandle,
					this->activDesc,
					&alpha,
					inputTensorDesc,
					this->m_lpInputBuffer_d,
					&beta,
					outputTensorDesc,
					thrust::raw_pointer_cast(&this->lpOutputBuffer_d[0]));
			}
			break;

			// softmax
			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_ALL:
			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_ALL_crossEntropy:
				{
					F32 alpha = 1.0f;
					F32 beta = 0.0f;
					cudnnStatus_t err =	cudnnSoftmaxForward(
						this->cudnnHandle,
						CUDNN_SOFTMAX_ACCURATE,
						CUDNN_SOFTMAX_MODE_INSTANCE,
						&alpha,
						this->inputTensorDesc,
						this->m_lpInputBuffer_d,
						&beta,
						this->outputTensorDesc,
						thrust::raw_pointer_cast(&this->lpOutputBuffer_d[0]));
				}
				break;
			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_CH:
			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_CH_crossEntropy:
				{
					F32 alpha = 1.0f;
					F32 beta = 0.0f;
					cudnnSoftmaxForward(
						this->cudnnHandle,
						CUDNN_SOFTMAX_ACCURATE,
						CUDNN_SOFTMAX_MODE_CHANNEL,
						&alpha,
						this->inputTensorDesc,
						this->m_lpInputBuffer_d,
						&beta,
						this->outputTensorDesc,
						thrust::raw_pointer_cast(&this->lpOutputBuffer_d[0]));
				}
				break;
		}

		return ErrorCode::ERROR_CODE_NONE;
	}


	/** 出力データバッファを取得する.
		配列の要素数はGetOutputBufferCountの戻り値.
		@return 出力データ配列の先頭ポインタ */
	CONST_BATCH_BUFFER_POINTER Activation_GPU::GetOutputBuffer()const
	{
		switch(this->layerData.layerStructure.ActivationType)
		{
			// lenear
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_lenear:
			return this->m_lpInputBuffer_d;
			break;

		default:
			return thrust::raw_pointer_cast(&this->lpOutputBuffer_d[0]);
			break;
		}
	}
	/** 出力データバッファを取得する.
		@param o_lpOutputBuffer	出力データ格納先配列. [GetBatchSize()の戻り値][GetOutputBufferCount()の戻り値]の要素数が必要
		@return 成功した場合0 */
	ErrorCode Activation_GPU::GetOutputBuffer(BATCH_BUFFER_POINTER o_lpOutputBuffer)const
	{
		if(o_lpOutputBuffer == NULL)
			return ErrorCode::ERROR_CODE_COMMON_NULL_REFERENCE;

		const U32 batchSize = this->GetBatchSize();
		const U32 outputBufferCount = this->GetOutputBufferCount();

		cudaMemcpy(o_lpOutputBuffer, this->GetOutputBuffer(), sizeof(F32)*outputBufferCount*batchSize, cudaMemcpyDeviceToHost);

		return ErrorCode::ERROR_CODE_NONE;
	}


	//================================
	// 学習処理
	//================================
	/** 学習誤差を計算する.
		入力信号、出力信号は直前のCalculateの値を参照する.
		@param	i_lppDOutputBuffer	出力誤差差分=次レイヤーの入力誤差差分.	[GetBatchSize()の戻り値][GetOutputBufferCount()の戻り値]の要素数が必要.
		直前の計算結果を使用する */
	ErrorCode Activation_GPU::CalculateLearnError(CONST_BATCH_BUFFER_POINTER i_lpDOutputBufferPrev)
	{
		// 出力誤差バッファのアドレスを配列に格納
		this->m_lpDOutputBufferPrev_d = i_lpDOutputBufferPrev;

		switch(this->layerData.layerStructure.ActivationType)
		{
			// lenear
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_lenear:
			break;

		default:
			// Sigmoid
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_sigmoid:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_sigmoid_crossEntropy:
			// ReLU
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_ReLU:
			// tanh
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_tanh:
			{
				F32 alpha = 1.0f;
				F32 beta = 0.0f;
				cudnnActivationBackward(
					this->cudnnHandle,
					this->activDesc,
					&alpha,
					this->outputTensorDesc,
					thrust::raw_pointer_cast(&this->lpDInputBuffer_d[0]),
					this->outputTensorDesc,
					this->m_lpDOutputBufferPrev_d,
					this->inputTensorDesc,
					this->m_lpInputBuffer_d,
					&beta,
					this->inputTensorDesc,
					thrust::raw_pointer_cast(&this->lpDInputBuffer_d[0])
					);
			}
			break;

			// softmax
			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_ALL:
				{
					F32 alpha = 1.0f;
					F32 beta = 0.0f;
					cudnnSoftmaxBackward(
						this->cudnnHandle,
						CUDNN_SOFTMAX_ACCURATE,
						CUDNN_SOFTMAX_MODE_INSTANCE,
						&alpha,
						this->outputTensorDesc,
						thrust::raw_pointer_cast(&this->lpOutputBuffer_d[0]),
						this->outputTensorDesc,
						this->m_lpDOutputBufferPrev_d,
						&beta,
						this->inputTensorDesc,
						thrust::raw_pointer_cast(&this->lpDInputBuffer_d[0])
						);
				}
				break;
			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_CH:
				{
					F32 alpha = 1.0f;
					F32 beta = 0.0f;
					cudnnSoftmaxBackward(
						this->cudnnHandle,
						CUDNN_SOFTMAX_ACCURATE,
						CUDNN_SOFTMAX_MODE_CHANNEL,
						&alpha,
						this->outputTensorDesc,
						thrust::raw_pointer_cast(&this->lpOutputBuffer_d[0]),
						this->outputTensorDesc,
						this->m_lpDOutputBufferPrev_d,
						&beta,
						this->inputTensorDesc,
						thrust::raw_pointer_cast(&this->lpDInputBuffer_d[0])
						);
				}
				break;

			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_ALL_crossEntropy:
			case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_CH_crossEntropy:
				break;
		}

		return ErrorCode::ERROR_CODE_NONE;
	}


	/** 学習差分をレイヤーに反映させる.
		入力信号、出力信号は直前のCalculateの値を参照する.
		出力誤差差分、入力誤差差分は直前のCalculateLearnErrorの値を参照する. */
	ErrorCode Activation_GPU::ReflectionLearnError(void)
	{
		return ErrorCode::ERROR_CODE_NONE;
	}

	/** 学習差分を取得する.
		配列の要素数は[GetBatchSize()の戻り値][GetInputBufferCount()の戻り値]
		@return	誤差差分配列の先頭ポインタ */
	CONST_BATCH_BUFFER_POINTER Activation_GPU::GetDInputBuffer()const
	{
		switch(this->layerData.layerStructure.ActivationType)
		{
			// lenear
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_lenear:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_sigmoid_crossEntropy:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_ALL_crossEntropy:
		case Gravisbell::Layer::NeuralNetwork::Activation::LayerStructure::ActivationType_softmax_CH_crossEntropy:
			return this->m_lpDOutputBufferPrev_d;
			break;

		default:
			return thrust::raw_pointer_cast(&this->lpDInputBuffer_d[0]);
			break;
		}
	}
	/** 学習差分を取得する.
		@param lpDInputBuffer	学習差分を格納する配列.[GetBatchSize()の戻り値][GetInputBufferCount()の戻り値]の配列が必要 */
	ErrorCode Activation_GPU::GetDInputBuffer(BATCH_BUFFER_POINTER o_lpDInputBuffer)const
	{
		if(o_lpDInputBuffer == NULL)
			return ErrorCode::ERROR_CODE_COMMON_NULL_REFERENCE;

		const U32 batchSize = this->GetBatchSize();
		const U32 inputBufferCount = this->GetInputBufferCount();

		cudaMemcpy(o_lpDInputBuffer, this->GetDInputBuffer(), sizeof(F32)*inputBufferCount*batchSize, cudaMemcpyDeviceToHost);

		return ErrorCode::ERROR_CODE_NONE;
	}


} // Gravisbell;
} // Layer;
} // NeuralNetwork;
