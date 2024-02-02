#pragma once
#include "Vector3.h"
#include <variant>
#include <map>
#include <string>
#include <json.hpp>

class GlobalVariables {
public:
	static GlobalVariables* GetInstance();

    /// <summary>
    /// グループの作成
    /// </summary>
    /// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	// 値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	// 値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	// 値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	 /// <summary>
	/// ファイルに書き出し
	/// </summary>
	/// <param name="groupName">グループ</param>
	void SaveFile(const std::string& groupName);

    /// <summary>
    /// 毎フレーム処理
    /// </summary>
	void Update();

private:
	// コンストラクタ
	GlobalVariables() = default;
	// デストラクタ
	~GlobalVariables() = default;
	// コピーコンストラクタ
	GlobalVariables(const GlobalVariables& obj) = delete;
	// コピー代入演算子
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;

	// 項目
	struct Item {
		// 項目の値
		std::variant<int32_t, float, Vector3> value;
	};
	// グループ
	struct Group {
		std::map<std::string, Item> items;
	};

	// 全データ
	std::map<std::string, Group> datas_;

	using json = nlohmann::json;

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";

	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	/// <param name="groupName">グループ</param>
	void LoadFiles(const std::string& groupName);

};
