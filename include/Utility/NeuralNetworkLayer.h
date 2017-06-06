//==================================
// ニューラルネットワークのレイヤー管理用のUtiltiy
// ライブラリとして使う間は有効.
// ツール化後は消す予定
//==================================
#ifndef __GRAVISBELL_UTILITY_NEURALNETWORKLAYER_H__
#define __GRAVISBELL_UTILITY_NEURALNETWORKLAYER_H__

#ifdef NEURALNETWORKLAYER_EXPORTS
#define GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API __declspec(dllexport)
#else
#define GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API __declspec(dllimport)
#ifndef GRAVISBELL_LIBRARY
#pragma comment(lib, "Gravisbell.Utility.NeuralNetworkLayer.lib")
#endif
#endif


#include"../Layer/NeuralNetwork/ILayerDLLManager.h"
#include"../Layer/Connect/ILayerConnectData.h"

#include<boost/filesystem.hpp>


namespace Gravisbell {
namespace Utility {
namespace NeuralNetworkLayer {

	
	/** レイヤーDLL管理クラスの作成(CPU用) */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::NeuralNetwork::ILayerDLLManager* CreateLayerDLLManagerCPU(const wchar_t i_libraryDirPath[]);
	/** レイヤーDLL管理クラスの作成(GPU用) */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::NeuralNetwork::ILayerDLLManager* CreateLayerDLLManagerGPU(const wchar_t i_libraryDirPath[]);

	//====================================
	// レイヤーデータを作成
	//====================================
	/** 複合ニューラルネットワーク.
		@param layerDLLManager	レイヤーDLL管理クラス.
		@param	inputDataStruct	入力データ構造. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::Connect::ILayerConnectData* CreateNeuralNetwork(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct);
	/** 畳込みニューラルネットワークレイヤー.
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	filterSize			フィルタサイズ.
		@param	outputChannelCount	フィルタの個数.
		@param	stride				フィルタの移動量.
		@param	paddingSize			パディングサイズ. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateConvolutionLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct, Vector3D<S32> filterSize, U32 outputChannelCount, Vector3D<S32> stride, Vector3D<S32> paddingSize);
	/** 全結合ニューラルネットワークレイヤー.
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	neuronCount			ニューロン数. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateFullyConnectLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct, U32 neuronCount);
	/** 活性化レイヤー.
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	activationType		活性化種別. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateActivationLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct, const std::wstring activationType);
	/** ドロップアウトレイヤー.
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	rate				ドロップアウト率.(0.0〜1.0).(0.0＝ドロップアウトなし,1.0=全入力無視) */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateDropoutLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct, F32 rate);
	/** プーリングレイヤー.
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	filterSize			プーリング幅.
		@param	stride				フィルタ移動量. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreatePoolingLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct, Vector3D<S32> filterSize, Vector3D<S32> stride);
	/** バッチ正規化レイヤー
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateBatchNormalizationLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct);
	/** 広域平均プーリングレイヤー
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateGlobalAveragePoolingLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct);
	/** アップサンプリングレイヤー
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	upScale				拡張率.
		@param	paddingUseValue		拡張部分の穴埋めに隣接する値を使用するフラグ. (true=UpConvolution, false=TransposeConvolution) */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateUpSamplingLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct, Vector3D<S32> upScale, bool paddingUseValue);

	/** 入力結合レイヤー. 入力されたレイヤーのCHを結合する. 入力データ構造はX,Y,Zで同じサイズである必要がある.
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	inputDataCount		入力されるレイヤーの個数. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateMergeInputLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct lpInputDataStruct[], U32 inputDataCount);
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateMergeInputLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const std::vector<IODataStruct>& lpInputDataStruct);
	template<typename... Rest>
	Layer::ILayerData* CreateMergeInputLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, std::vector<IODataStruct>& lpInputDataStruct, const IODataStruct& inputDataStruct_first, const Rest&... lpInputDataStruct_rest)
	{
		lpInputDataStruct.push_back(inputDataStruct_first);

		return CreateMergeInputLayer(layerDLLManager, lpInputDataStruct, lpInputDataStruct_rest...);
	}
	template<typename... Rest>
	Layer::ILayerData* CreateMergeInputLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct_first, const Rest&... lpInputDataStruct_rest)
	{
		std::vector<IODataStruct> lpInputDataStruct;
		lpInputDataStruct.push_back(inputDataStruct_first);

		return CreateMergeInputLayer(layerDLLManager, lpInputDataStruct, lpInputDataStruct_rest...);
	}


	/** 入力結合レイヤー. 入力されたレイヤーの値を合算する. 出力されるレイヤーのサイズは全サイズのうちの最大値になる.
		@param	layerDLLManager		レイヤーDLL管理クラス.
		@param	inputDataStruct		入力データ構造.
		@param	inputDataCount		入力されるレイヤーの個数. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateResidualLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct lpInputDataStruct[], U32 inputDataCount);
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Layer::ILayerData* CreateResidualLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const std::vector<IODataStruct>& lpInputDataStruct);
	template<typename... Rest>
	Layer::ILayerData* CreateResidualLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, std::vector<IODataStruct>& lpInputDataStruct, const IODataStruct& inputDataStruct_first, const Rest&... lpInputDataStruct_rest)
	{
		lpInputDataStruct.push_back(inputDataStruct_first);

		return CreateResidualLayer(layerDLLManager, lpInputDataStruct, lpInputDataStruct_rest...);
	}
	template<typename... Rest>
	Layer::ILayerData* CreateResidualLayer(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, const IODataStruct& inputDataStruct_first, const Rest&... lpInputDataStruct_rest)
	{
		std::vector<IODataStruct> lpInputDataStruct;
		lpInputDataStruct.push_back(inputDataStruct_first);

		return CreateResidualLayer(layerDLLManager, lpInputDataStruct, lpInputDataStruct_rest...);
	}


	/** レイヤーをネットワークの末尾に追加する.GUIDは自動割り当て.入力データ構造、最終GUIDも更新する. */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Gravisbell::ErrorCode AddLayerToNetworkLast(
		Layer::Connect::ILayerConnectData& neuralNetwork,
		std::list<Layer::ILayerData*>& lppLayerData, Gravisbell::IODataStruct& inputDataStruct, Gravisbell::GUID& lastLayerGUID, Layer::ILayerData* pAddlayer);

	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Gravisbell::ErrorCode AddLayerToNetworkLast(
		Layer::Connect::ILayerConnectData& neuralNetwork,
		std::list<Layer::ILayerData*>& lppLayerData, Gravisbell::IODataStruct& inputDataStruct, Gravisbell::GUID& lastLayerGUID, Layer::ILayerData* pAddLayer,
		const Gravisbell::GUID lpInputLayerGUID[], U32 inputLayerCount);
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Gravisbell::ErrorCode AddLayerToNetworkLast(
		Layer::Connect::ILayerConnectData& neuralNetwork,
		std::list<Layer::ILayerData*>& lppLayerData, Gravisbell::IODataStruct& inputDataStruct, Gravisbell::GUID& lastLayerGUID, Layer::ILayerData* pAddLayer,
		const std::vector<Gravisbell::GUID>& lpInputLayerGUID);
	template<typename... Rest>
	Gravisbell::ErrorCode AddLayerToNetworkLast(
		Layer::Connect::ILayerConnectData& neuralNetwork,
		std::list<Layer::ILayerData*>& lppLayerData, Gravisbell::IODataStruct& inputDataStruct, Gravisbell::GUID& lastLayerGUID, Layer::ILayerData* pAddlayer,
		std::vector<Gravisbell::GUID>& lpInputLayerGUID,
		const Gravisbell::GUID& lastLayerGUID_first, const Rest&... lpLastLayerGUID_rest)
	{
		lpInputLayerGUID.push_back(lastLayerGUID_first);
	
		return AddLayerToNetworkLast(neuralNetwork, lppLayerData, inputDataStruct, lastLayerGUID, pAddlayer, lpInputLayerGUID, lpLastLayerGUID_rest...);
	}
	template<typename... Rest>
	Gravisbell::ErrorCode AddLayerToNetworkLast(
		Layer::Connect::ILayerConnectData& neuralNetwork,
		std::list<Layer::ILayerData*>& lppLayerData, Gravisbell::IODataStruct& inputDataStruct, Gravisbell::GUID& lastLayerGUID, Layer::ILayerData* pAddlayer,
		const Gravisbell::GUID& lastLayerGUID_first, const Rest&... lpLastLayerGUID_rest)
	{
		std::vector<Gravisbell::GUID> lpInputLayerGUID;
		lpInputLayerGUID.push_back(lastLayerGUID_first);

		return AddLayerToNetworkLast(neuralNetwork, lppLayerData, inputDataStruct, lastLayerGUID, pAddlayer, lpInputLayerGUID, lpLastLayerGUID_rest...);
	}


	/** ニューラルネットワークをバイナリファイルに保存する */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Gravisbell::ErrorCode WriteNetworkToBinaryFile(const Layer::ILayerData& neuralNetwork, const wchar_t i_filePath[]);
	/** ニューラルネットワークをバイナリファイルから読み込むする */
	GRAVISBELL_UTILITY_NEURALNETWORKLAYER_API Gravisbell::ErrorCode ReadNetworkFromBinaryFile(const Layer::NeuralNetwork::ILayerDLLManager& layerDLLManager, Layer::ILayerData** ppNeuralNetwork, const wchar_t i_filePath[]);


}	// NeuralNetworkLayer
}	// Utility
}	// Gravisbell

#endif

