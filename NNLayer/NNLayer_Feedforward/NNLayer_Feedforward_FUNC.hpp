/*--------------------------------------------
 * FileName  : NNLayer_Feedforward_DATA.hpp
 * LayerName : 全結合ニューラルネットワークレイヤー
 * guid      : BEBA34EC-C30C-4565-9386-56088981D2D7
 * 
 * Text      : 全結合ニューラルネットワークレイヤー.
 *           : 結合層と活性化層を一体化.
 *           : 学習時に[学習係数][ドロップアウト率]を設定できる.
--------------------------------------------*/
#ifndef __GRAVISBELL_NEURALNETWORK_LAYER_FUNC_NNLayer_Feedforward_H__
#define __GRAVISBELL_NEURALNETWORK_LAYER_FUNC_NNLayer_Feedforward_H__

#define EXPORT_API extern "C" __declspec(dllexport)

#include<Common/guiddef.h>
#include<Common/ErrorCode.h>
#include<Common/VersionCode.h>

#include<NNLayerInterface/ILayerConfig.h>
#include<NNLayerInterface/INNLayer.h>

#include"NNLayer_Feedforward_DATA.hpp"


/** Acquire the layer identification code.
  * @param  o_layerCode    Storage destination buffer.
  * @return On success 0. 
  */
EXPORT_API Gravisbell::ErrorCode GetLayerCode(GUID& o_layerCode);

/** Get version code.
  * @param  o_versionCode    Storage destination buffer.
  * @return On success 0. 
  */
EXPORT_API Gravisbell::ErrorCode GetVersionCode(Gravisbell::VersionCode& o_versionCode);


/** Create a layer structure setting.
  * @return If successful, new configuration information.
  */
EXPORT_API Gravisbell::NeuralNetwork::ILayerConfig* CreateLayerStructureSetting(void);

/** Create layer structure settings from buffer.
  * @param  i_lpBuffer       Start address of the read buffer.
  * @param  i_bufferSize     The size of the readable buffer.
  * @param  o_useBufferSize  Buffer size actually read.
  * @return If successful, the configuration information created from the buffer
  */
EXPORT_API Gravisbell::NeuralNetwork::ILayerConfig* CreateLayerStructureSettingFromBuffer(const BYTE* i_lpBuffer, int i_bufferSize, int& o_useBufferSize);


/** Create a learning setting.
  * @return If successful, new configuration information. */
EXPORT_API Gravisbell::NeuralNetwork::ILayerConfig* CreateLearningSetting(void);

/** Create learning settings from buffer.
  * @param  i_lpBuffer       Start address of the read buffer.
  * @param  i_bufferSize     The size of the readable buffer.
  * @param  o_useBufferSize  Buffer size actually read.
  * @return If successful, the configuration information created from the buffer
  */
EXPORT_API Gravisbell::NeuralNetwork::ILayerConfig* CreateLearningSettingFromBuffer(const BYTE* i_lpBuffer, int i_bufferSize, int& o_useBufferSize);


/** Create a layer for CPU processing.
  * @param GUID of layer to create.
  */
EXPORT_API Gravisbell::NeuralNetwork::INNLayer* CreateLayerCPU(GUID guid);

/** Create a layer for GPU processing.
  * @param GUID of layer to create.
  */
EXPORT_API Gravisbell::NeuralNetwork::INNLayer* CreateLayerGPU(GUID guid);



#endif // __GRAVISBELL_NEURAULNETWORK_LAYER_FUNC_NNLayer_Feedforward_H__
