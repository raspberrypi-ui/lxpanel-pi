"""
  Copyright (c) 2010 Julien Lavergne <gilir@ubuntu.com>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
"""

import os
import apport.hookutils

#Detect session
session = os.environ['DESKTOP_SESSION']

#If it's not Lubuntu or LXDE, fallback to default location
if session != "Lubuntu" and session != "LXDE":
    session = "default"

#Set location of various configuration files
system_conf = "/usr/share/lxpanel/profile/"
home_conf = os.path.expanduser("~/.config/lxpanel/")

#Set description for each file reported by apport
report_config_system = "Config_System_" + session
report_panel_system = "Panel_System_" + session
report_config_home = "Config_Home_" + session
report_panel_home = "Panel_Home_" + session

def add_info(report):
    # If a config file exist in HOME, report it instead of the system one.
    if os.path.exists(os.path.join(home_conf,session,"config")):
        report[report_config_home] = apport.hookutils.read_file(os.path.join(home_conf,session,"config"))
    else:
        report[report_config_system] = apport.hookutils.read_file(os.path.join(system_conf,session,"config"))
    
    if os.path.exists(os.path.join(home_conf,session,"panels/panel")):
        report[report_panel_home] = apport.hookutils.read_file(os.path.join(home_conf,session,"panels/panel"))
    else:
        report[report_panel_system] = apport.hookutils.read_file(os.path.join(system_conf,session,"panels/panel"))
