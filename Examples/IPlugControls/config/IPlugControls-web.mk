# IPLUG2_ROOT should point to the top level IPLUG2 folder from the project folder
# By default, that is three directories up from /Examples/IPlugControls/config
IPLUG2_ROOT = ../../..

include ../../../common-web.mk

SRC += $(PROJECT_ROOT)/IPlugControls.cpp

# WAM_SRC +=

# WAM_CFLAGS +=

WEB_CFLAGS += -DIGRAPHICS_NANOVG -DIGRAPHICS_GLES2

WAM_LDFLAGS += -O3 -s --closure 0 EXPORT_NAME="'AudioWorkletGlobalScope.WAM.IPlugControls'" -s ASSERTIONS=0

WEB_LDFLAGS += -O3 -s ASSERTIONS=0 --closure 1 --pre-js ../build-web/imgs.js --pre-js ../build-web/imgs@2x.js --pre-js ../build-web/fonts.js --pre-js ../build-web/svgs.js

WEB_LDFLAGS += $(NANOVG_LDFLAGS)
