#!/bin/bash

FONT_PATH="NotoSansMono-Regular.ttf"
TEXT="Hello, world!"

convert -debug annotate xc: -font "$FONT_PATH" -annotate 0 "$TEXT" null: 2>&1 | grep -E 'Metrics: text:' | awk '{print $0}'
