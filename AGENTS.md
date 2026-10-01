# 專案記憶與合作原則

此檔記錄使用者確認的專案方向與合作方式。後續在本專案工作時，請先讀取並遵守。

## 唯讀參考資料

- `readonly/`（使用者亦稱 `ReadOnly`）及其所有內容只能讀取，是參考資料。
- 不得在此目錄內新增、修改、刪除、移動或重新命名任何檔案或目錄。
- 不得將建置產物、快取、產生的程式碼或工具輸出寫入此目錄。
- 實作與建置須在 `readonly/` 以外進行；實際位置待實作範圍確定後再決定。

## 專案目標

- 建立以 `readonly/00-SourceCode` 這個版本為基礎的 C++ 專案。
- 設備內容與功能方向參考 `readonly/WLS3200-OLS`，目標是類似該設備的專案。
- 尚未透過閱讀原始碼確認的架構、工具鏈、功能與移植方式，不得當成已確認的需求。

## 架構與參考邊界

- 進行設計或實作前，先讀取 [架構討論紀錄](docs/architecture.md)，區分已確認方向、原始碼觀察與待討論事項。
- 新專案是晶圓檢測設備。`00-SourceCode` 是 PCBA 檢測專案，提供 C++／VS2015 工具鏈與底層實作參考；`WLS3200-OLS` 提供晶圓設備的領域與架構參考。
- 不因沿用 JET8000 的版本規格，就直接沿用其應用架構；不得將進出板、板軌、PCB 停板位置等不適用的需求帶入新專案。
- 不把 `CMotion_Basic`／`CMotionAxis` 整體當成新架構骨架。按當次需求評估 SDK 呼叫、硬體映射與錯誤處理等可參考內容。
- `JET8000_Library` 是相依套件與執行環境的集合，不等同純核心契約層；不整包引入，依實際硬體與功能選用。
- 運動部分以使用者偏好的 YuanliCore 方向展開：控制器管理多個 Axis，Axis 提供單軸操作並委派控制器執行硬體命令。具體 API、生命週期與等待行為尚未定案。

## 小步實作與溝通

- 一次只完成一個小部分，例如一個 class，不一次產生大量程式碼或推進多個模組。
- 每次實作前，先說明本次小部分的目的、範圍，以及與既有程式的關係，讓使用者能掌握設計。
- 若需要較大範圍的變更，先拆成小步驟，本次只實作其中一個。
- 完成後說明修改內容與驗證結果，讓使用者理解後再接續下一部分；不要自行連續實作後續部分。
- 合作重點是維持使用者的心智模型與程式碼同步，而非追求一次完成大量功能。
- 程式註解使用簡短繁體中文；EFEM、Aligner、SDK 等專業用語及程式識別名稱可保留英文。

## 已確認的版本與建置規格（2026-10-01）

使用者要求：團隊使用 C++ 與 Visual Studio 2015，所有後續程式碼與專案設定必須符合此環境，不得自行升級工具鏈。

### 參考來源與版本

- 原始碼基準目錄：`readonly/00-SourceCode/JET8000-v1.01.04.064#20260803_01_AddSaveFrameToCacheFolder/`。
- 採用其中的 `JET8000_VS2015.sln` 與 `JET8000_VS2015.vcxproj` 作為建置規格依據；同目錄的 VS2010 專案不是本次目標。
- `VersionDefine.h` 註記 `JET8000-v1.01.04.064`，`AOI3D_APP_VERSION` 與 `AOI3D_FILE_VERSION` 均為 `1,1,4,64`。
- `JET8000.rc` 的 FILEVERSION／PRODUCTVERSION 仍是 `1,1,4,44`，與上述版本不一致；保留此差異的紀錄，不修改參考資料。
- `VersionDefine.h` 目前啟用 `OFFLINE_VERSION`；這是參考原始碼現況，不代表新設備必須採離線模式。

### 建置設定依據

- `.sln`：Visual Studio 14，`VisualStudioVersion = 14.0.25420.1`。
- `.vcxproj`：目標工具集 `v140`、`WindowsTargetPlatformVersion = 8.1`；`targetver.h` 將 `VS_2015_NET` 定義為 `1900`。
- 方案實際列出的組態皆為 x64：`Debug_Unicode`、`Release_Unicode`、`Debug_Release`。專案另保留 Win32 與其他組態，不能因此認定都是本次支援目標。
- 以上三個 x64 組態皆為原生 C++ Windows Application、靜態 MFC（`UseOfMfc=Static`）、Unicode。
- CRT：`Debug_Unicode|x64` 使用 `MultiThreadedDebug`（/MTd）；`Release_Unicode|x64` 與 `Debug_Release|x64` 使用 `MultiThreaded`（/MT）。`Debug_Release` 定義 NDEBUG，不能按名稱當作一般 Debug CRT 組態。
- 使用 `stdafx.h` 預編譯標頭；三個組態均開啟 OpenMP。
- 原專案匯入 CUDA 9.1 的 `.props` 與 `.targets`；這是原專案的建置相依性，新專案是否需要 CUDA 須依實際功能判定。

### 後續實作的相容規則

- 使用 VS2015／MSVC v140 能編譯的 C++ 語法與標準函式庫。以此編譯器的實際支援為準，不假定完整支援所有 C++11／C++14 功能。
- 不使用 C++17／C++20 或更新功能，例如 `std::filesystem`、`std::optional`、`std::string_view`、結構化綁定、`if constexpr`、concepts、ranges。
- 原專案未明確設定 `LanguageStandard`；不得自行加上較新語言標準或把專案升級為 v141／v142／v143。
- 建立專案時以 VS2015、v140、Windows SDK 8.1、x64、Unicode、靜態 MFC 與上述 CRT 配置為基準。新增第三方程式庫前，先確認工具集、架構與 CRT 相容性。
- 配合既有 MFC／Unicode 介面與預編譯標頭慣例。新增檔案時也須注意 VS2015 的文字編碼相容性。
- 不可直接建置唯讀目錄中的方案：原設定會將中間檔與執行檔寫入 `readonly/`。未來在可寫入的實作目錄建置，並確認所有輸出路徑皆在唯讀目錄外。
- 本次只確認檔案內的規格，尚未驗證本機已安裝 VS2015、SDK、CUDA 或完整相依套件，也未執行編譯；不得宣稱已通過 VS2015 建置驗證。

## 目前進度

- 已建立合作原則與專案方向的本地記憶。
- 已讀取 VS2015 方案、專案設定、`targetver.h`、`stdafx.h` 與版本定義，完成版本及建置規格的初步確認。
- 已初步對照運動抽象與軸物件，並記錄 PCBA／晶圓設備的參考邊界；尚未進行完整功能架構分析。
- 第一個實作為 `src/Motion/MotionResult.h`，包含 `MotionError` 與 `MotionResult`。
- 已建立 `src/Motion/IMotionController.h` 抽象契約，提供 MoveAbsolute、GetActualPosition、IsMotionDone、Stop，統一回傳 MotionResult。尚未建立 Axis 或具體控制器；軸集合與生命週期待設計 Axis 時討論。
- 已建立根目錄的 `WaferInspectionSystem.sln`，供 IDE 瀏覽。當前 C++ 專案為 Utility，尚無執行程式；使用 v140／SDK 8.1／x64，輸出限制在 `build/`。已通過 MSBuild 14.0 的 Debug_Unicode|x64 建置流程（不含程式編譯）。
