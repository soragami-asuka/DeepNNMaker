//======================================
// フィードフォワードニューラルネットワークの処理レイヤーのデータ
// 複数のレイヤーを内包し、処理する
//======================================
#include"stdafx.h"

#include"FeedforwardNeuralNetwork_FUNC.hpp"
#include"FeedforwardNeuralNetwork_LayerData_Base.h"
#include"FeedforwardNeuralNetwork_GPU_d.cuh"
#include"FeedforwardNeuralNetwork_GPU_h.cuh"

using namespace Gravisbell;

namespace Gravisbell {
namespace Layer {
namespace NeuralNetwork {

	class FeedforwardNeuralNetwork_LayerData_GPU : public FeedforwardNeuralNetwork_LayerData_Base
	{	
		//====================================
		// コンストラクタ/デストラクタ
		//====================================
	public:
		/** コンストラクタ */
		FeedforwardNeuralNetwork_LayerData_GPU(const ILayerDLLManager& i_layerDLLManager, const Gravisbell::GUID& guid)
			:	FeedforwardNeuralNetwork_LayerData_Base(i_layerDLLManager, guid)
		{
		}
		/** デストラクタ */
		virtual ~FeedforwardNeuralNetwork_LayerData_GPU()
		{
		}

		//===========================
		// レイヤー作成
		//===========================
		/** レイヤーを作成する.
			@param guid	新規生成するレイヤーのGUID. */
		ILayerBase* CreateLayer(const Gravisbell::GUID& guid, const IODataStruct i_lpInputDataStruct[], U32 i_inputLayerCount, Gravisbell::Common::ITemporaryMemoryManager& i_temporaryMemoryManager)
		{
			return this->CreateLayer(guid, i_lpInputDataStruct, i_inputLayerCount, i_temporaryMemoryManager, false);
		}
		/** レイヤーを作成する.
			@param	guid	新規生成するレイヤーのGUID.
			@param	i_lpInputDataStruct	入力データ構造の配列. GetInputFromLayerCount()の戻り値以上の要素数が必要
			@param	i_useHostMemory		内部的にホストメモリを使用する. */
		ILayerBase* CreateLayer(const Gravisbell::GUID& guid, const IODataStruct i_lpInputDataStruct[], U32 i_inputLayerCount, Gravisbell::Common::ITemporaryMemoryManager& i_temporaryMemoryManager, bool i_useHostMemory)
		{
			if(this->CheckCanUseInputDataStruct(i_lpInputDataStruct, i_inputLayerCount) == false)
				return NULL;

			FeedforwardNeuralNetwork_Base* pNeuralNetwork = NULL;
			if(i_useHostMemory)
				pNeuralNetwork = new FeedforwardNeuralNetwork_GPU_h(guid, *this, i_lpInputDataStruct[0], i_temporaryMemoryManager);
			else
				pNeuralNetwork = new FeedforwardNeuralNetwork_GPU_d(guid, *this, i_lpInputDataStruct[0], i_temporaryMemoryManager);

			// ニューラルネットワークにレイヤーを追加
			ErrorCode err = AddConnectionLayersToNeuralNetwork(*pNeuralNetwork, i_lpInputDataStruct, i_inputLayerCount);
			if(err != ErrorCode::ERROR_CODE_NONE)
			{
				delete pNeuralNetwork;
				return NULL;
			}

			return pNeuralNetwork;
		}


		/** レイヤーを作成する.
			@param guid	新規生成するレイヤーのGUID. */
		ILayerBase* CreateLayer(const Gravisbell::GUID& guid, const IODataStruct i_lpInputDataStruct[], U32 i_inputLayerCount)
		{
			return this->CreateLayer(guid, i_lpInputDataStruct, i_inputLayerCount, false);
		}
		/** レイヤーを作成する.
			@param	guid	新規生成するレイヤーのGUID.
			@param	i_lpInputDataStruct	入力データ構造の配列. GetInputFromLayerCount()の戻り値以上の要素数が必要
			@param	i_useHostMemory		内部的にホストメモリを使用する. */
		ILayerBase* CreateLayer(const Gravisbell::GUID& guid, const IODataStruct i_lpInputDataStruct[], U32 i_inputLayerCount, bool i_useHostMemory)
		{
			if(this->CheckCanUseInputDataStruct(i_lpInputDataStruct, i_inputLayerCount) == false)
				return NULL;
			
			FeedforwardNeuralNetwork_Base* pNeuralNetwork = NULL;
			if(i_useHostMemory)
				pNeuralNetwork = new FeedforwardNeuralNetwork_GPU_h(guid, *this, i_lpInputDataStruct[0]);
			else
				pNeuralNetwork = new FeedforwardNeuralNetwork_GPU_d(guid, *this, i_lpInputDataStruct[0]);

			// ニューラルネットワークにレイヤーを追加
			ErrorCode err = AddConnectionLayersToNeuralNetwork(*pNeuralNetwork, i_lpInputDataStruct, i_inputLayerCount);
			if(err != ErrorCode::ERROR_CODE_NONE)
			{
				delete pNeuralNetwork;
				return NULL;
			}

			return pNeuralNetwork;
		}
	};


}	// NeuralNetwork
}	// Layer
}	// Gravisbell


/** Create a layer for CPU processing.
  * @param GUID of layer to create.
  */
EXPORT_API Gravisbell::Layer::ILayerData* CreateLayerDataGPU(const Gravisbell::Layer::NeuralNetwork::ILayerDLLManager* pLayerDLLManager, Gravisbell::GUID guid, const Gravisbell::SettingData::Standard::IData& i_data)
{
	// DLLマネージャのNULLチェック
	if(pLayerDLLManager == NULL)
		return NULL;

	// 作成
	Gravisbell::Layer::NeuralNetwork::FeedforwardNeuralNetwork_LayerData_GPU* pLayerData = new Gravisbell::Layer::NeuralNetwork::FeedforwardNeuralNetwork_LayerData_GPU(*pLayerDLLManager, guid);
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
EXPORT_API Gravisbell::Layer::ILayerData* CreateLayerDataGPUfromBuffer(const Gravisbell::Layer::NeuralNetwork::ILayerDLLManager* pLayerDLLManager, Gravisbell::GUID guid, const BYTE* i_lpBuffer, S32 i_bufferSize, S32& o_useBufferSize)
{
	// DLLマネージャのNULLチェック
	if(pLayerDLLManager == NULL)
		return NULL;

	// 作成
	Gravisbell::Layer::NeuralNetwork::FeedforwardNeuralNetwork_LayerData_GPU* pLayerData = new Gravisbell::Layer::NeuralNetwork::FeedforwardNeuralNetwork_LayerData_GPU(*pLayerDLLManager, guid);
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
