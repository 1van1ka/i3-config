#!/usr/bin/env bash

dir="$HOME/.config/rofi/windows"
theme='style-2.rasi'

rofi \
    -show window \
    -theme ${dir}/${theme}
