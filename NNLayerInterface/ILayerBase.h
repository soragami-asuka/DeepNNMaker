//=======================================
// レイヤーベース
//=======================================
#ifndef __GRAVISBELL_I_LAYER_BASE_H__
#define __GRAVISBELL_I_LAYER_BASE_H__

#include<guiddef.h>

#include"Common/Common.h"
#include"Common/ErrorCode.h"
#include"Common/IODataStruct.h"

#include"ILayerConfig.h"

namespace Gravisbell {
namespace NeuralNetwork {

	/** レイヤー種別 */
	enum ELayerKind
	{
		LAYER_KIND_CPU = 0x00 << 16,	/**< CPU処理レイヤー */
		LAYER_KIND_GPU = 0x01 << 16,	/**< GPU処理レイヤー */

		LAYER_KIND_SINGLE_INPUT  = 0x01 << 0,	/**< 入力レイヤー */
		LAYER_KIND_MULT_INPUT    = 0x01 << 1,	/**< 入力レイヤー */
		LAYER_KIND_SINGLE_OUTPUT = 0x01 << 2,	/**< 出力レイヤー */
		LAYER_KIND_MULT_OUTPUT   = 0x01 << 3,	/**< 出力レイヤー */

		LAYER_KIND_CALC          = 0x01 << 8,	/**< 計算レイヤー,中間層 */
		LAYER_KIND_DATA			 = 0x02 << 8,	/**< データレイヤー.入出力層 */
	};


	/** レイヤーベース */
	class ILayerBase
	{
	public:
		/** コンストラクタ */
		ILayerBase(){}
		/** デストラクタ */
		virtual ~ILayerBase(){}

	public:
		/** レイヤー種別の取得.
			ELayerKind の組み合わせ. */
		virtual unsigned int GetLayerKind()const = 0;

		/** レイヤー固有のGUIDを取得する */
		virtual ErrorCode GetGUID(GUID& o_guid)const = 0;

		/** レイヤー種別識別コードを取得する.
			@param o_layerCode	格納先バッファ
			@return 成功した場合0 */
		virtual ErrorCode GetLayerCode(GUID& o_layerCode)const = 0;

	public:
		/** 演算前処理を実行する.(学習用)
			@param batchSize	同時に演算を行うバッチのサイズ.
			NN作成後、演算処理を実行する前に一度だけ必ず実行すること。データごとに実行する必要はない.
			失敗した場合はPreProcessLearnLoop以降の処理は実行不可. */
		virtual ErrorCode PreProcessLearn(unsigned int batchSize) = 0;

		/** 演算前処理を実行する.(演算用)
			@param batchSize	同時に演算を行うバッチのサイズ.
			NN作成後、演算処理を実行する前に一度だけ必ず実行すること。データごとに実行する必要はない.
			失敗した場合はCalculate以降の処理は実行不可. */
		virtual ErrorCode PreProcessCalculate(unsigned int batchSize) = 0;

		/** 学習ループの初期化処理.データセットの学習開始前に実行する
			失敗した場合はCalculate以降の処理は実行不可. */
		virtual ErrorCode PreProcessLearnLoop(const ILayerConfig& config) = 0;


		/** バッチサイズを取得する.
			@return 同時に演算を行うバッチのサイズ */
		virtual unsigned int GetBatchSize()const = 0;
	};

}	// NeuralNetwork
}	// Gravisbell

#endif