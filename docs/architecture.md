# 晶圓檢測設備架構討論紀錄

更新日期：2026-10-01。此文件保存討論依據，不表示所有設計均已定案。工具鏈與合作規則見 [AGENTS.md](../AGENTS.md)。

## 已確認的背景與方向

- `readonly/00-SourceCode` 是 PCBA 檢測專案；`readonly/WLS3200-OLS` 是晶圓檢測專案。
- 新專案使用 C++／VS2015，設備方向為晶圓檢測，涉及 EFEM、平台移動與相機。具體型號、組成與流程尚待確認。
- JET8000 提供工具鏈與底層實作參考，WLS3200-OLS 提供晶圓設備領域與架構參考；兩者都不直接整套移植。
- 使用者擔心 PCBA 應用需求與底層控制過度耦合。進出板、板軌與 PCB 停板位置等內容不納入新專案。
- 先從運動子系統討論。使用者傾向 YuanliCore 的設計：Axis 提供單軸操作，MotionController 作為控制器抽象，管理多個 Axis。
- 持續採小步實作，一次只完成一個小部分；目前只建立運動呼叫結果型別。

## 第一個實作：MotionResult

- IDE 入口：根目錄 `WaferInspectionSystem.sln`。`Motion` 篩選器包含目前的標頭，`Docs` 方案資料夾包含記憶與架構文件。
- 目前只有標頭，專案暫用 Utility 類型，不產生 EXE，也不建立預編譯標頭；未來建立可執行程式時再配置入口與 stdafx.h。工具鏈保留 VS2015 v140、SDK 8.1 與三個 x64 組態，輸出位於唯讀目錄外的 build/。

- 使用者已選擇以結果型別回報呼叫結果，並同意建立 `MotionResult.h`。
- 檔案位於 `src/Motion/MotionResult.h`，包含 `MotionError` 與 `MotionResult`，無 MFC 或 SDK 相依。
- `MotionError` 提供 None、NotInitialized、InvalidAxis、InvalidArgument、Busy、Unsupported、DriverError 分類。
- `MotionResult` 包含 `error`、`nativeErrorCode` 與 `IsSuccess()`；預設為成功、原始錯誤碼為 0。
- 成功表示本次呼叫成功，不代表實體運動已完成。上層依統一錯誤分類判斷，原始 SDK 錯誤碼供診斷使用。
- 原始錯誤碼目前使用 int；接入實際 SDK 時須確認是否能完整保存其代碼。
- IMotionController 的四個候選方法為 MoveAbsolute、GetActualPosition、IsMotionDone、Stop，目前尚未建立該介面。
- 目前命名採型別與方法 PascalCase、參數與公開資料欄位 camelCase；介面型別使用 I 前綴。

## 原始碼觀察

以下是已讀取檔案的觀察，不代表完整架構審查或硬體驗證。

### YuanliCore

來源根目錄：`readonly/WLS3200-OLS/YuanliCore/YuanliCore.Model/`。

- `Interface/Motion/Axis.cs`：Axis 接收 IMotionController 與軸號。單軸操作委派控制器，例如 MoveToAsync 呼叫 MoveToCommand；Axis 另處理部分限位檢查、等待與狀態。
- `Interface/IMotioncontroller.cs`：契約提供 Axes 集合與帶軸號的命令。
- `Motion/Adlink/Adlink7856.cs`：具體控制器建立 `new Axis(this, info.AxisID)`，並將操作轉為 SDK 呼叫。
- 整個 YuanliCore 不只是介面集合，也包含設備實作、資料模型、UI 與應用模組。參考其責任分工，不表示原樣移植 C# 非同步或等待邏輯。

### JET8000

來源根目錄：`readonly/00-SourceCode/JET8000-v1.01.04.064#20260803_01_AddSaveFrameToCacheFolder/`。

- `Motion Device/Motion_Basic.h`：CMotion_Basic 是抽象基底，包含 `std::vector<CMotionAxis>`，並提供 Home、Enable、Disable、MoveTo、WaitForDone、GetEncode 等契約。
- `Motion Device/MotionAxis.h`：CMotionAxis 主要包裝軸參數與節點資料，不是 YuanliCore 那種提供 MoveTo 操作並委派控制器的單軸物件。
- `Motion Device/MotionNode.h`：包含 CardID、NodeID、SlotID 等映射資訊。不能直接假定一個 CMotion_Basic 只代表一張實體卡。
- CMotionAxis／CMotionNode 混有進出板速度、PCB 停板位置等 PCBA 應用設定，不適合整體搬入新專案。
- `Motion Device/Motion_Lib_Module.cpp`：可參考其 mouMoveAbsolute、mouMoveRelative、mouExecHomeEx 等呼叫方式。
- `JET8000_Library/MotionModule` 提供模組 API、包裝程式與 DLL；是否重用，要確認實際控制卡、版本、部署與行為。

## 建議的責任邊界（待逐步落實）

這是目前討論基準，不是已核定的完整類別清單。

| 元件 | 責任 | 不放入的內容 |
|---|---|---|
| 軸卡實作 | SDK 呼叫、卡與軸映射、命令與狀態轉換 | 晶圓流程、PCBA 進出板 |
| Axis | 單軸操作、軸號與必要設定 | 晶圓交接或檢測步驟的意義 |
| 控制器抽象 | 硬體命令契約、軸集合的存取 | 特定廠商 SDK 型別向應用層擴散 |
| 平台 | 多軸組合、平台座標與協調移動 | 完整上下料與檢測排程 |
| 相機 | 連線、曝光、取像、觸發 | 晶圓檢測位置與流程決策 |
| EFEM 子系統 | 依實際配置封裝設備操作與狀態 | 假定所有元件都應抽象成 Axis |
| 設備流程 | 晶圓交接、定位、取像與檢測順序，以及跨子系統互鎖 | 直接操作廠商 SDK |

例如晶圓交接位置由上層定義，Axis 只接收目標位置。EFEM 的 Load Port、Robot、Aligner 是可能的組成例子，目前尚未確認實際配置。

預期呼叫方向：設備流程／UI → Axis → 控制器契約 → 具體控制器 → 選用的 MotionModule 或廠商 SDK。控制器同時管理 Axis 集合；這是組合與委派關係，不是繼承鏈。

## 重用判斷方式

對每次要參考的功能，先確認：

1. 新設備現在需要哪些能力？
2. 重用時會帶入哪些類別、全域狀態、設定檔、UI 或二進位相依？
3. 移除 PCBA、板軌、進出板與配方等概念後，該功能是否仍成立？
4. 能否以小封裝接入新契約，並驗證正常與失敗行為？

- SDK 呼叫、讀取編碼器、命令轉換與錯誤碼處理等列為候選參考內容，不表示已確認可以直接重用。
- 若底層與應用邏輯混雜，理解底層行為後依新契約重新實作，避免整個 class 搬入。
- 第三方 SDK 與既有 DLL 只在需求、硬體與建置相容時引入；目前不估算整體重用比例。
- 不因參考專案具有某功能就預先建置；不一次建立完整通用設備框架。

## 待討論與驗證

- 實際控制卡、驅動器與軸配置；一個控制器物件對應單卡、多卡或通訊系統的範圍。
- 第一個單軸操作情境，以及最小契約。
- MoveTo 表示命令送出或等待到位；完成、停止、逾時與錯誤如何表達。
- 單位、座標、軟限位、回原點、狀態與等待邏輯各由哪層負責。
- C++ 物件生命週期；建議控制器擁有 Axis、Axis 非擁有地參照控制器，但尚未定案。
- 多軸同時下命令與插補／同步運動的需求區別。
- 重用 MotionModule 或直接包裝廠商 SDK；尚未進行硬體或 DLL 驗證。

上述事項逐步討論，不必全部定案後才實作第一個小部分，但不能把建議當成已確認規格。
