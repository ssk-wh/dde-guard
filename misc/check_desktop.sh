#!/bin/bash
user=$1

echo "current user:" $user

compare_file_sizes() {
    local app_dir="/usr/share/applications/"
    local desktop_dir="/home/$user/Desktop/"

    local files=("dde-computer.desktop" "dde-trash.desktop" "dde-home.desktop" "deepin-tooltips.desktop")

    for file in "${files[@]}"; do
        if [ -f "${app_dir}${file}" ]; then
            if [ ! -f "${desktop_dir}${file}" ]; then
                echo "文件 ${file} 不存在于桌面目录"
                exit 1
            else
                size_app=$(stat -c%s "${app_dir}${file}")
                size_desktop=$(stat -c%s "${desktop_dir}${file}")

                if [ "$size_app" -ne "$size_desktop" ]; then
                    echo "文件 ${file} 大小不一致"
                    exit 1
                fi
            fi
        else
            echo "文件 ${file} 不存在于应用目录"
        fi
    done

    echo "所有文件大小一致"
}

compare_file_sizes
