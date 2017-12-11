//======================================
// 全結合ニューラルネットワークのレイヤーデータ
// CPU制御
//======================================
#include"stdafx.h"

#include"FullyConnect_LayerData_CPU.h"
#include"FullyConnect_FUNC.hpp"
#include"FullyConnect_CPU.h"

#include"Library/NeuralNetwork/Optimizer.h"
#include"Library/NeuralNetwork/Initializer.h"

namespace Gravisbell {
namespace Layer {
namespace NeuralNetwork {


	//===========================
	// コンストラクタ / デストラクタ
	//===========================
	/** コンストラクタ */
	FullyConnect_LayerData_CPU::FullyConnect_LayerData_CPU(const Gravisbell::GUID& guid)
		:	FullyConnect_LayerData_Base(guid)
	{
	}
	/** デストラクタ */
	FullyConnect_LayerData_CPU::~FullyConnect_LayerData_CPU()
	{
	}


	//===========================
	// 初期化
	//===========================
	/** 初期化. 各ニューロンの値をランダムに初期化
		@return	成功した場合0 */
	ErrorCode FullyConnect_LayerData_CPU::Initialize(void)
	{
		// 入力バッファ数を確認
		unsigned int inputBufferCount = this->GetInputBufferCount();
		if(inputBufferCount == 0)
			return ErrorCode::ERROR_CODE_COMMON_OUT_OF_VALUERANGE;

		// ニューロン数を確認
		unsigned int neuronCount = this->GetNeuronCount();
		if(neuronCount == 0)
			return ErrorCode::ERROR_CODE_COMMON_OUT_OF_VALUERANGE;

		// バッファを確保しつつ、初期値を設定
		auto& initializer = Gravisbell::Layer::NeuralNetwork::GetInitializerManager().GetInitializer(this->layerStructure.Initializer);
		U32 inputCount  = inputBufferCount;
		U32 outputCount = neuronCount;

		this->lpNeuron.resize(neuronCount * inputBufferCount);
		this->lppNeuron.resize(neuronCount);
		this->lpBias.resize(neuronCount);

		// ニューロン
		for(unsigned int neuronNum=0; neuronNum<neuronCount; neuronNum++)
		{
			lppNeuron[neuronNum] = &this->lpNeuron[inputBufferCount * neuronNum];
			for(unsigned int inputNum=0; inputNum<inputBufferCount; inputNum++)
			{
				this->lppNeuron[neuronNum][inputNum] = initializer.GetParameter(inputCount, outputCount);
			}
		}
		// バイアス
		for(unsigned int neuronNum=0; neuronNum<neuronCount; neuronNum++)
		{
			this->lpBias[neuronNum] = initializer.GetParameter(inputCount, outputCount);
		}

		return ErrorCode::ERROR_CODE_NONE;
	}
	/** 初期化. 各ニューロンの値をランダムに初期化
		@param	i_config			設定情報
		@oaram	i_inputDataStruct	入力データ構造情報
		@return	成功した場合0 */
	ErrorCode FullyConnect_LayerData_CPU::Initialize(const SettingData::Standard::IData& i_data)
	{
		ErrorCode err;

		// 設定情報の登録
		err = this->SetLayerConfig(i_data);
		if(err != ErrorCode::ERROR_CODE_NONE)
			return err;

		// 初期化
		err = this->Initialize();
		if(err != ErrorCode::ERROR_CODE_NONE)
			return err;

		// オプティマイザーの設定
		err = this->ChangeOptimizer(L"SGD");
		if(err != ErrorCode::ERROR_CODE_NONE)
			return err;

		return ErrorCode::ERROR_CODE_NONE;
	}
	/** 初期化. バッファからデータを読み込む
		@param i_lpBuffer	読み込みバッファの先頭アドレス.
		@param i_bufferSize	読み込み可能バッファのサイズ.
		@return	成功した場合0 */
	ErrorCode FullyConnect_LayerData_CPU::InitializeFromBuffer(const BYTE* i_lpBuffer, U32 i_bufferSize, S32& o_useBufferSize)
	{
		int readBufferByte = 0;

		// 設定情報
		S32 useBufferByte = 0;
		SettingData::Standard::IData* pLayerStructure = CreateLayerStructureSettingFromBuffer(&i_lpBuffer[readBufferByte], i_bufferSize, useBufferByte);
		if(pLayerStructure == NULL)
			return ErrorCode::ERROR_CODE_INITLAYER_READ_CONFIG;
		readBufferByte += useBufferByte;
		this->SetLayerConfig(*pLayerStructure);
		delete pLayerStructure;

		// 初期化する
		this->Initialize();

		// ニューロン係数
		memcpy(&this->lpNeuron[0], &i_lpBuffer[readBufferByte], this->lpNeuron.size() * sizeof(NEURON_TYPE));
		readBufferByte += (int)this->lpNeuron.size() * sizeof(NEURON_TYPE);

		// バイアス
		memcpy(&this->lpBias[0], &i_lpBuffer[readBufferByte], this->lpBias.size() * sizeof(NEURON_TYPE));
		readBufferByte += (int)this->lpBias.size() * sizeof(NEURON_TYPE);


		// オプティマイザ
		S32 useBufferSize = 0;
		// bias
		if(this->m_pOptimizer_bias)
			delete this->m_pOptimizer_bias;
		this->m_pOptimizer_bias = CreateOptimizerFromBuffer_CPU(&i_lpBuffer[readBufferByte], i_bufferSize-readBufferByte, useBufferSize);
		readBufferByte += useBufferSize;
		// neuron
		if(this->m_pOptimizer_neuron)
			delete this->m_pOptimizer_neuron;
		this->m_pOptimizer_neuron = CreateOptimizerFromBuffer_CPU(&i_lpBuffer[readBufferByte], i_bufferSize-readBufferByte, useBufferSize);
		readBufferByte += useBufferSize;


		o_useBufferSize = readBufferByte;

		return ErrorCode::ERROR_CODE_NONE;
	}


	//===========================
	// レイヤー保存
	//===========================
	/** レイヤーをバッファに書き込む.
		@param o_lpBuffer	書き込み先バッファの先頭アドレス. GetUseBufferByteCountの戻り値のバイト数が必要
		@return 成功した場合書き込んだバッファサイズ.失敗した場合は負の値 */
	S32 FullyConnect_LayerData_CPU::WriteToBuffer(BYTE* o_lpBuffer)const
	{
		if(this->pLayerStructure == NULL)
			return ErrorCode::ERROR_CODE_NONREGIST_CONFIG;

		int writeBufferByte = 0;

		// 設定情報
		writeBufferByte += this->pLayerStructure->WriteToBuffer(&o_lpBuffer[writeBufferByte]);

		// ニューロン係数
		memcpy(&o_lpBuffer[writeBufferByte], &this->lpNeuron[0], this->lpNeuron.size() * sizeof(NEURON_TYPE));
		writeBufferByte += (int)this->lpNeuron.size() * sizeof(NEURON_TYPE);

		// バイアス
		memcpy(&o_lpBuffer[writeBufferByte], &this->lpBias[0], this->lpBias.size() * sizeof(NEURON_TYPE));
		writeBufferByte += (int)this->lpBias.size() * sizeof(NEURON_TYPE);

		// オプティマイザ
		// bias
		writeBufferByte += this->m_pOptimizer_bias->WriteToBuffer(&o_lpBuffer[writeBufferByte]);
		// neuron
		writeBufferByte += this->m_pOptimizer_neuron->WriteToBuffer(&o_lpBuffer[writeBufferByte]);


		return writeBufferByte;
	}


	//===========================
	// レイヤー作成
	//===========================
	/** レイヤーを作成する.
		@param guid	新規生成するレイヤーのGUID. */
	ILayerBase* FullyConnect_LayerData_CPU::CreateLayer(const Gravisbell::GUID& guid, const IODataStruct i_lpInputDataStruct[], U32 i_inputLayerCount, Gravisbell::Common::ITemporaryMemoryManager& i_temporaryMemoryManager)
	{
		if(this->CheckCanUseInputDataStruct(i_lpInputDataStruct, i_inputLayerCount) == false)
			return NULL;

		return new FullyConnect_CPU(guid, *this, i_lpInputDataStruct[0]);
	}
	

	//===========================
	// オプティマイザー設定
	//===========================
	/** オプティマイザーを変更する */
	ErrorCode FullyConnect_LayerData_CPU::ChangeOptimizer(const wchar_t i_optimizerID[])
	{
		ChangeOptimizer_CPU(&this->m_pOptimizer_bias,   i_optimizerID, (U32)this->lpBias.size());
		ChangeOptimizer_CPU(&this->m_pOptimizer_neuron, i_optimizerID, (U32)this->lpNeuron.size());

		return ErrorCode::ERROR_CODE_NONE;
	}

} // Gravisbell;
} // Layer;
} // NeuralNetwork;


/** Create a layer for CPU processing.
  * @param GUID of layer to create.
  */
EXPORT_API Gravisbell::Layer::ILayerData* CreateLayerDataCPU(const Gravisbell::Layer::NeuralNetwork::ILayerDLLManager* pLayerDLLManager, Gravisbell::GUID guid, const Gravisbell::SettingData::Standard::IData& i_data)
{
	// 作成
	Gravisbell::Layer::NeuralNetwork::FullyConnect_LayerData_CPU* pLayerData = new Gravisbell::Layer::NeuralNetwork::FullyConnect_LayerData_CPU(guid);
	if(pLayerData == NULL)
		return NULL;

	// 初期化
	Gravisbell::ErrorCode errCode = pLayerData->Initialize(i_data);
	if(errCode != Gravisbell::ErrorCode::ERROR_CODE_NONE)
	{
		delete pLayerData;
		return NULL;
	}

	return pLayerData;
}
EXPORT_API Gravisbell::Layer::ILayerData* CreateLayerDataCPUfromBuffer(const Gravisbell::Layer::NeuralNetwork::ILayerDLLManager* pLayerDLLManager, Gravisbell::GUID guid, const BYTE* i_lpBuffer, S32 i_bufferSize, S32& o_useBufferSize)
{
	// 作成
	Gravisbell::Layer::NeuralNetwork::FullyConnect_LayerData_CPU* pLayerData = new Gravisbell::Layer::NeuralNetwork::FullyConnect_LayerData_CPU(guid);
	if(pLayerData == NULL)
		return NULL;

	// 初期化
	S32 useBufferSize = 0;
	Gravisbell::ErrorCode errCode = pLayerData->InitializeFromBuffer(i_lpBuffer, i_bufferSize, useBufferSize);
	if(errCode != Gravisbell::ErrorCode::ERROR_CODE_NONE)
	{
		delete pLayerData;
		return NULL;
	}

	// 使用したバッファ量を格納
	o_useBufferSize = useBufferSize;

	return pLayerData;
}
