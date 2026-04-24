menu "App (gh_app) configuration"

config GH_CHIP_TYPE
	int "Chip type (GH_CHIP_TYPE)"
    range 0 3
	default 3
    help
        0: GH3018
        1: GH3X2X
        2: GH33XX
        3: GH3036

config GH_FUNC_ADT_EN
	bool "Enable ADT function (GH_FUNC_ADT_EN)"
	default y

config GH_FUNC_HR_EN
	bool "Enable HR function (GH_FUNC_HR_EN)"
	default y

config GH_FUNC_SPO2_EN
	bool "Enable SPO2 function (GH_FUNC_SPO2_EN)"
	default y

config GH_FUNC_HRV_EN
	bool "Enable HRV function (GH_FUNC_HRV_EN)"
	default y

config GH_FUNC_GNADT_EN
	bool "Enable GNADT function (GH_FUNC_GNADT_EN)"
	default y

config GH_FUNC_IRNADT_EN
	bool "Enable IRNADT function (GH_FUNC_IRNADT_EN)"
	default n

config GH_FUNC_TEST1_EN
	bool "Enable TEST1 function (GH_FUNC_TEST1_EN)"
	default y

config GH_FUNC_TEST2_EN
	bool "Enable TEST2 function (GH_FUNC_TEST2_EN)"
	default y

config GH_FUNC_SLOT_EN
	bool "Enable SLOT function (GH_FUNC_SLOT_EN)"
	default n

config GH_USE_DYNAMIC_ALGO_MEM
	bool "Enable dynamic algo memory (GH_USE_DYNAMIC_ALGO_MEM)"
	default n

config GH_FUSION_MODE_SEL
	int "Fusion mode (GH_FUSION_MODE_SEL)"
    range 0 2
	default 1
    help
        0: none fusion mode
        1: sync fusion mode
        2: async fusion mode

config GH_MTSS_EN
	bool "Enable MTSS (GH_MTSS_EN)"
	default y

config GH_MOVE_GH_WEAR_ON_ITVAL_TIME
    int "G-sensor wear-on interval time (GH_MOVE_GH_WEAR_ON_ITVAL_TIME) (unit: second)"
    default 3000
    depends on GH_MTSS_EN

config GH_GS_MOVE_DET_EN
	bool "Enable g-sensor move detect (GH_GS_MOVE_DET_EN)"
	default y

config GH_GSENSOR_DEBUG_EN
	bool "Enable g-sensor debug (GH_GSENSOR_DEBUG_EN)"
	default n

config GH_DEMO_DATA_LOG_EN
	bool "Enable demo data log (GH_DEMO_DATA_LOG_EN)"
	default y

config GH_APP_MIPS_STA_EN
	bool "Enable app MIPS statistics (GH_APP_MIPS_STA_EN)"
	default n

config GH_FIFO_READ_MAX_NUM
	int "Max FIFO read items (GH_FIFO_READ_MAX_NUM)"
	default 255

endmenu
