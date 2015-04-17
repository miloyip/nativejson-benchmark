#!/bin/sh
premake4 gmake
sed -i '' 's/,-x//' gmake/*.make