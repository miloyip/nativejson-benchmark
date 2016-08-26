#!/bin/bash
premake5 gmake

# This is probably necessary only on OSX because premake generates
#  invalid compiler flags. See:
#  https://github.com/miloyip/nativejson-benchmark/issues/25#issuecomment-174154308
if [[ "$OSTYPE" == "darwin"* ]]; then
	sed -i.bak 's/,-x//' gmake/*.make
	sed -i.bak 's/-Wl,--start-group//' gmake/*.make
	sed -i.bak 's/-Wl,--end-group//' gmake/*.make
fi
