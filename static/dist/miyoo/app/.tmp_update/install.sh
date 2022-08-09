#!/bin/sh
installdir=`cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P`
core_zipfile="$installdir/onion.pak"
ra_zipfile="/mnt/SDCARD/RetroArch/retroarch.pak"
ra_version_file="/mnt/SDCARD/RetroArch/onion_ra_version.txt"
ra_package_version_file="/mnt/SDCARD/RetroArch/ra_package_version.txt"

# globals
install_ra=0
total_core=0
total_ra=0
total_all=0
install_core_begin=0
install_core_offset=50
install_ra_begin=50
install_ra_offset=50


main() {
    # init_lcd
    cat /proc/ls
    sleep 0.25
    
    # init charger detection
    gpiodir=/sys/devices/gpiochip0/gpio
    if [ ! -f $gpiodir/gpio59/direction ]; then
        echo 59 > /sys/class/gpio/export
        echo "in" > $gpiodir/gpio59/direction
    fi

    # init backlight
    pwmdir=/sys/class/pwm/pwmchip0
    echo 0  	> $pwmdir/export
    echo 800    > $pwmdir/pwm0/period
    echo 80     > $pwmdir/pwm0/duty_cycle
    echo 1  	> $pwmdir/pwm0/enable

    if [ ! -d /mnt/SDCARD/.tmp_update/onionVersion ]; then
        fresh_install 1
        cleanup
        return
    fi

    cd $installdir

    # Start the battery monitor
    ./bin/batmon 2>&1 > ./logs/batmon.log &

    # Prompt for update or fresh install
    ./bin/prompt -r -m "Welcome to the Onion installer!\nPlease choose an action:" \
        "Update" \
        "Repair (keep settings)" \
        "Reinstall (reset settings)"
    retcode=$?

    if [ $retcode -eq 0 ]; then
        # Update
        update_only
    elif [ $retcode -eq 1 ]; then
        # Repair (keep settings)
        fresh_install 0
    elif [ $retcode -eq 2 ]; then
        # Reinstall (reset settings)
        fresh_install 1
    else
        # Cancel (can be reached if pressing POWER)
        return
    fi

    cleanup
}

cleanup() {
    echo ":: Cleanup"
    cd $installdir
    rm -f \
        /tmp/.update_msg \
        .installed \
        removed.png \
        removed \
        install.sh
}

get_install_stats() {
    install_ra=$(check_install_retroarch)
    total_core=$(zip_total "$core_zipfile")
    total_ra=0

    if [ $install_ra -eq 1 ] && [ -f "$ra_zipfile" ]; then
        total_ra=$(zip_total "$ra_zipfile")
    fi

    total_all=$(($total_core + $total_ra))

    install_core_begin=0
    install_core_offset=$(($total_core / $total_all * 100))
    install_ra_begin=$install_core_offset
    install_ra_offset=$((100 - $install_core_offset))
}

remove_configs() {
    echo ":: Remove configs"
    rm -rf \
        /mnt/SDCARD/RetroArch/.retroarch/retroarch.cfg
        /mnt/SDCARD/Saves/CurrentProfile/config/*
        /mnt/SDCARD/Saves/GuestProfile/config/*
}

fresh_install() {
    reset_configs=$1
    echo ":: Fresh install (reset: $reset_configs)"

    get_install_stats

    rm -f /tmp/.update_msg

    # Show installation progress
    cd $installdir
    ./bin/installUI -b $install_core_begin -t $install_core_offset &
    sleep 1

    # Backup important stock files
    echo "Backing up files..." >> /tmp/.update_msg
    backup_system

    echo "Uninstalling old system..." >> /tmp/.update_msg

    if [ $reset_configs -eq 1 ]; then
        remove_configs
        maybe_remove_retroarch
    fi

    # Debloat the apps folder
    debloat_apps
    refresh_roms

    # Remove stock folders
    cd /mnt/SDCARD
    rm -rf Emu/* RApp/* Imgs miyoo

    install_core "Installing Onion..."

    if [ $install_ra -eq 1 ]; then
        free_ui_switch_to_ra # frees memory
        install_retroarch "Installing RetroArch..."
    fi

    echo "Completing installation..." >> /tmp/.update_msg
    if [ $reset_configs -eq 0 ]; then
        restore_ra_config
    fi
    install_configs $reset_configs
    
    echo "Installation complete!" >> /tmp/.update_msg

    touch $installdir/.installed
    sync

    cd /mnt/SDCARD/App/Onion_Manual/
    ./launch.sh
    free_mma

    # Launch layer manager
    cd /mnt/SDCARD/App/The_Onion_Installer/ 
    ./packageManager
    free_mma

    # display turning off message
    cd /mnt/SDCARD/App/Onion_Manual
    ./removed

    cd $installdir
    ./config/boot_mod.sh 

    if [ $reset_configs -eq 1 ]; then
        cp -f $installdir/config/system.json /appconfigs/system.json
    fi
}

update_only() {
    echo ":: Update only"

    get_install_stats

    # Show installation progress
    cd $installdir
    ./bin/installUI -b $install_core_begin -t $install_core_offset -m "Preparing update..." &
    sleep 1

    install_core "Updating Onion..."

    if [ $install_ra -eq 1 ]; then
        free_ui_switch_to_ra # frees memory
        install_retroarch "Updating RetroArch..."
        restore_ra_config
    fi

    install_configs 0

    echo "Update complete! Turning off..." >> /tmp/.update_msg

    touch $installdir/.installed
    sync
}

free_ui_switch_to_ra() {
    touch $installdir/.installed
    sync

    # Free memory
    free_mma

    rm -f $installdir/.installed
    rm -f /tmp/.update_msg

    # Show installation progress for RetroArch
    cd $installdir
    ./bin/installUI -b $install_ra_begin -t $install_ra_offset &
    sleep 1
}

install_core() {
    echo ":: Install Onion"
    msg="$1"

    if [ ! -f "$core_zipfile" ]; then
        return
    fi

    rm -f $installdir/updater

    echo "$msg 0%" >> /tmp/.update_msg

    # Onion core installation / update
    unzip_progress "$core_zipfile" "$msg" /mnt/SDCARD $total_core

    if [ $? -ne 0 ]; then
        touch $installdir/.installFailed
        echo Onion - installation failed
        exit 0
    fi

    rm -f $core_zipfile
}

check_install_retroarch() {
    install_ra=1

    # An existing version of Onion's RetroArch exist
    if [ -f $ra_version_file ] && [ -f $ra_package_version_file ]; then
        current_ra_version=`cat $ra_version_file`
        package_ra_version=`cat $ra_package_version_file`

        # Skip installation if current version is up-to-date
        if [ $(version $current_ra_version) -ge $(version $package_ra_version) ]; then
            install_ra=0
        fi
    fi

    return "$install_ra"
}

install_retroarch() {
    echo ":: Install RetroArch"
    msg="$1"

    # Check if RetroArch zip also exists
    if [ ! -f "$ra_zipfile" ]; then
        return
    fi

    echo "$msg 0%" >> /tmp/.update_msg

    # Backup old RA configuration
    cd /mnt/SDCARD/RetroArch
    mv .retroarch/retroarch.cfg /mnt/SDCARD/Backup/

    # Remove old RetroArch before unzipping
    maybe_remove_retroarch
    
    # Install RetroArch
    unzip_progress "$ra_zipfile" "$msg" /mnt/SDCARD $total_ra
    
    if [ $? -ne 0 ]; then
        touch $installdir/.installFailed
        echo RetroArch - installation failed
        exit 0
    fi

    rm -f $ra_zipfile $ra_package_version_file
}

maybe_remove_retroarch() {
    if [ -f $ra_zipfile ]; then
        cd /mnt/SDCARD/RetroArch
        remove_everything_except `basename $ra_zipfile`
    fi
}

restore_ra_config() {
    echo ":: Restore RA config"
    cfg_file=/mnt/SDCARD/Backup/retroarch.cfg
    if [ -f $cfg_file ]; then
        mv -f $cfg_file /mnt/SDCARD/RetroArch/.retroarch/
    fi
}

install_configs() {
    reset_configs=$1
    zipfile=$installdir/config/configs.pak

    echo ":: Install configs (reset: $reset_configs)"

    if [ ! -f $zipfile ]; then
        return
    fi

    cd /mnt/SDCARD
    if [ $reset_configs -eq 1 ]; then
        # Overwrite all default configs
        unzip -oq $zipfile
    else
        # Extract config files without overwriting any existing files
        unzip -nq $zipfile
    fi
}

check_firmware() {
    echo ":: Check firmware"
    if [ ! -f /customer/lib/libpadsp.so ]; then
        cd $installdir
        ./removed
        reboot
        exit 0
    fi
}

backup_system() {
    echo ":: Backup system"
    old_ra_dir=/mnt/SDCARD/RetroArch/.retroarch

    # Move BIOS files from stock location
    if [ -d $old_ra_dir/system ] ; then
        mkdir -p /mnt/SDCARD/BIOS
        cp -R $old_ra_dir/system/. /mnt/SDCARD/BIOS/
    fi

    # Backup old saves
    if [ -d $old_ra_dir/saves ] ; then
        mkdir -p /mnt/SDCARD/Backup/saves
        cp -R $old_ra_dir/saves/. /mnt/SDCARD/Backup/saves/
    fi    

    # Backup old states
    if [ -d $old_ra_dir/states ] ; then
        mkdir -p /mnt/SDCARD/Backup/states
        cp -R $old_ra_dir/states/. /mnt/SDCARD/Backup/states/
    fi

    # Themes
    if [ -d /mnt/SDCARD/Themes ]; then
        mv -f /mnt/SDCARD/Themes/* /mnt/SDCARD/Backup/Themes
    fi

    # Imgs
    if [ -d /mnt/SDCARD/Imgs ]; then
        mv -f /mnt/SDCARD/Imgs/* /mnt/SDCARD/Backup/Imgs
    fi
}

debloat_apps() {
    echo ":: Debloat apps"
    cd /mnt/SDCARD/App
    rm -rf \
        Commander_CN \
        power \
        RetroArch \
        swapskin \
        Pal \
        OpenBor \
        Onion_Manual \
        PlayActivity \
        Retroarch \
        The_Onion_Installer \
        Clean_View_Toggle \
        StartGameSwitcher \
        Guest_Mode \
        ThemeSwitcher
}

refresh_roms() {
    echo ":: Refresh roms"
    # Force refresh the rom lists
    if [ -d /mnt/SDCARD/Roms ] ; then
        cd /mnt/SDCARD/Roms
        find . -type f -name "*.db" -exec rm -f {} \;
    fi
}

version() {
    echo "$@" | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }';
}

remove_everything_except() {
    find * .* -maxdepth 0 -not -name "$1" -exec rm -rf {} \;
}

zip_total() {
    zipfile="$1"
    total=`unzip -l "$zipfile" | tail -1 | grep -Eo "([0-9]+) files" | sed "s/[^0-9]*//g"`
    echo $total
}

unzip_progress() {
    zipfile="$1"
    msg="$2"
    dest="$3"
    total=$4

    echo "   - Extract '$zipfile' ($total files) into $dest"

    unzip -o "$zipfile" -d "$dest" | awk -v total="$total" -v out="/tmp/.update_msg" -v msg="$msg" 'BEGIN { cnt = 0; l = 0; printf "" > out; }{
        p = int(cnt * 100 / total);
        if (p != l) {
            printf "%s %3.0f%%\n", msg, p >> out;
            close(out);
            l = p;
        }
        cnt += 1;
    }'

    echo "$msg 100%" >> /tmp/.update_msg
}

free_mma() {
    /mnt/SDCARD/.tmp_update/bin/freemma
}

main
sync
reboot
sleep 10