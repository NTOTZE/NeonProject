# NeonProject

> **Unreal Engine 5 기반 3인 파티 실시간 액션 게임**

캐릭터를 교체하며 콤보, 회피, 스킬, 궁극기를 연계하는 실시간 전투 프로젝트입니다.  
전투 루프와 게임 흐름을 C++ 중심으로 설계하고, 블루프린트는 데이터 구성과 콘텐츠 연결에 활용했습니다.

## Demo

[![NeonProject 플레이 영상](https://img.youtube.com/vi/5zD0aCDS7Ng/maxresdefault.jpg)](https://youtu.be/5zD0aCDS7Ng)

> 썸네일을 클릭하면 플레이 영상으로 이동합니다.

## 프로젝트 개요

| 구분 | 내용 |
| --- | --- |
| 엔진 | Unreal Engine 5.4 |
| 언어 | C++ / Blueprint |
| 플랫폼 | Windows 64-bit |
| 장르 | 3인 파티 기반 실시간 액션 |
| 핵심 목표 | 캐릭터 교체와 전투 상태 전환이 자연스럽게 이어지는 전투 경험 구현 |

## 주요 구현

### 전투 및 캐릭터

- 콤보 입력 윈도우, 공격·피격·무적 등 상태 플래그를 기반으로 한 전투 흐름 구현
- 회피, 대시, 락온, 투사체, 범위 피해 액터를 포함한 액션 시스템 구현
- 애니메이션 노티파이를 이용해 공격 판정, 콤보 가능 구간, 이동 제한을 게임 플레이와 동기화
- `NPPartyComponent`를 통해 3인 파티의 실시간 캐릭터 교체와 전투 정보 갱신 처리

### 적 AI 및 전투 스테이지

- Behavior Tree 기반 적 AI와 거리·확률·상태 조건 Decorator 및 Task 구현
- `NPEncounterManager`와 Spawn Point를 이용한 인카운터 및 웨이브 전투 진행
- 몬스터, 플레이어 캐릭터, 스테이지 데이터를 DataTable 및 DataAsset으로 분리

### 게임 흐름 및 UI

- 시작 화면 → 허브 → 스테이지 선택 → 전투 → 결과 화면으로 이어지는 게임 흐름 구성
- 로딩, 맵 전환, 스테이지 세션을 Subsystem과 Handler로 분리
- 파티 상태, HP, 스킬 자원, 웨이브 및 전투 결과를 UMG UI에 연동
- 궁극기 연출을 위한 Level Sequence 및 컷신 보조 시스템 구현

## 코드 구성

| 영역 | 주요 경로 |
| --- | --- |
| 전투 | `Source/NeonProject/Combat` |
| 캐릭터·파티·스탯 | `Source/NeonProject/Character`, `Component` |
| AI | `Source/NeonProject/AI` |
| 게임 흐름·로딩 | `Source/NeonProject/GameFlow`, `Loading` |
| 데이터 | `Source/NeonProject/DataAsset`, `DataType` |
| UI | `Source/NeonProject/UI`, `Screen` |
| 블루프린트·맵·DataTable | `Content/NeonProject` |

## 실행 방법

플레이 가능한 Windows 배포 패키지는 [GitHub Releases](https://github.com/NTOTZE/NeonProject/releases/latest)에서 제공합니다.

1. 최신 Release의 `NeonProject_Portfolio_Win64.zip.part-000`부터 `part-007`까지 모두 다운로드합니다.
2. 다운로드한 파일이 있는 폴더에서 명령 프롬프트를 열고 아래 명령으로 압축 파일을 복원합니다.

   ```bat
   copy /b NeonProject_Portfolio_Win64.zip.part-000 + NeonProject_Portfolio_Win64.zip.part-001 + NeonProject_Portfolio_Win64.zip.part-002 + NeonProject_Portfolio_Win64.zip.part-003 + NeonProject_Portfolio_Win64.zip.part-004 + NeonProject_Portfolio_Win64.zip.part-005 + NeonProject_Portfolio_Win64.zip.part-006 + NeonProject_Portfolio_Win64.zip.part-007 NeonProject_Portfolio_Win64.zip
   ```

3. `NeonProject_Portfolio_Win64.zip`을 압축 해제한 뒤 `Windows/NeonProject.exe`를 실행합니다.

## 소스 저장소 안내

- 이 저장소에는 C++ 소스, 프로젝트 설정, 직접 제작한 블루프린트·맵·UI·DataTable을 포함합니다.
- Unreal Engine 바이너리 에셋(`.uasset`, `.umap`)은 Git LFS로 관리됩니다. 클론 환경에 Git LFS가 필요합니다.
- Marketplace 및 외부에서 내려받은 원본 에셋은 라이선스 및 저장소 용량 문제로 포함하지 않습니다.
- 따라서 소스만 클론한 환경에서는 외부 에셋 참조가 누락될 수 있으며, 플레이 가능한 결과물은 Releases 패키지를 사용해야 합니다.

## 개발 환경

- Unreal Engine 5.4
- Visual Studio 2022
- Windows 64-bit
