Hacks on top of i3 to make it compatible with Xfce
==================================================

Provided features compared to upstream i3:

  - a small utility program (called `sm`) to announce i3's startup to xfce4-session, to get rid of the session management timeout (https://github.com/i3/i3/issues/4844)
  - an `xfce4-session.xml` that is tested and it starts a new xfce session with i3 QUICKLY without timeouts (https://github.com/i3/i3/pull/4843)
  - a way to use i3bar together with xfce4-panel (i3bar is used for workspace selection, xfce4-panel for everything else)

Debian packages to install for this repo
----------------------------------------

For xfce: `apt-get install --no-install-recommends xfce4 xfce4-goodies gnome-keyring`

The `no-install-recommends` is just there to appease people, who are
minimalist, if you are a normal person, feel free to leave it out.

The `gnome-keyring` package sounds bad, but it is necessary, because
Google Chrome unfortunately depends on it if it detects the Xfce
session based on the `DESKTOP_SESSION=xfce` environment variable.
(Alternative to installing the bloated `gnome-keyring` is to unset
this variable during your Google Chrome startup somehow.)  If you
don't use chrome or chromium, or other broken software from Google,
you can leave `gnome-keyring` out.

Build dependencies: `apt-get build-dep i3-wm`

Building this repo with meson and ninja
---------------------------------------

```
mkdir build
cd build
meson ..
ninja
```

Build should be pretty fast, i3 is a lean repo to compile.

Utility program sm
------------------

Just put this at the end of your `~/.i3/config`:

```
exec ~/nilcons-i3-xfce/build/sm/sm
```

This will provide the necessary workaround for https://github.com/i3/i3/issues/4844

Correct `xfce4-session` config for i3 with xfce
-----------------------------------------------

We are defining a new session name, instead of overriding the default
failsafe, so there is no need to delete the xfdesktop and other
components of xfce, that are incompatible with i3.

Put this in your `~/.config/xfce4/xfconf/xfce-perchannel-xml/xfce4-session.xml`
(make sure that when you do this edit, you don't have a running xfce
session, or a leftover `xfconfd` program in your `ps -ef`):

```
<?xml version="1.0" encoding="UTF-8"?>

<channel name="xfce4-session" version="1.0">
  <property name="splash" type="empty">
    <property name="Engine" type="empty"/>
    <property name="engines" type="empty">
      <property name="simple" type="empty">
        <property name="Font" type="empty"/>
        <property name="BgColor" type="empty"/>
        <property name="FgColor" type="empty"/>
        <property name="Image" type="empty"/>
      </property>
    </property>
  </property>
  <property name="general" type="empty">
    <property name="FailsafeSessionName" type="string" value="i3withxfce"/>
    <property name="LockCommand" type="string" value="xnilconslock"/>
    <property name="SessionName" type="string" value="Default"/>
    <property name="SaveOnExit" type="bool" value="false"/>
  </property>
  <property name="sessions" type="empty">
    <property name="Failsafe" type="empty">
      <property name="IsFailsafe" type="empty"/>
      <property name="Count" type="empty"/>
      <property name="Client0_Command" type="empty"/>
      <property name="Client0_Priority" type="empty"/>
      <property name="Client0_PerScreen" type="empty"/>
      <property name="Client1_Command" type="empty"/>
      <property name="Client1_Priority" type="empty"/>
      <property name="Client1_PerScreen" type="empty"/>
      <property name="Client2_Command" type="empty"/>
      <property name="Client2_Priority" type="empty"/>
      <property name="Client2_PerScreen" type="empty"/>
      <property name="Client3_Command" type="empty"/>
      <property name="Client3_Priority" type="empty"/>
      <property name="Client3_PerScreen" type="empty"/>
      <property name="Client4_Command" type="empty"/>
      <property name="Client4_Priority" type="empty"/>
      <property name="Client4_PerScreen" type="empty"/>
    </property>
    <property name="i3withxfce" type="empty">
      <property name="IsFailsafe" type="bool" value="true"/>
      <property name="Count" type="int" value="3"/>
      <property name="Client0_Command" type="array">
        <value type="string" value="i3"/>
      </property>
      <property name="Client0_Priority" type="int" value="5"/>
      <property name="Client0_PerScreen" type="bool" value="false"/>
      <property name="Client1_Command" type="array">
        <value type="string" value="xfsettingsd"/>
      </property>
      <property name="Client1_Priority" type="int" value="10"/>
      <property name="Client1_PerScreen" type="bool" value="false"/>
      <property name="Client2_Command" type="array">
        <value type="string" value="xfce4-panel"/>
      </property>
      <property name="Client2_Priority" type="int" value="15"/>
      <property name="Client2_PerScreen" type="bool" value="false"/>
    </property>
  </property>
</channel>
```

Next, leave your X session and login in a Linux VT, kill `xfconfd`,
and `rm -rf ~/.cache/sessions`.  The reason for this, is that failsafe
is only executed, if we don't have a previously saved xfce session, so
we have to delete the last one.  New ones will not be created, because
we disabled that in the the XML with `SaveOnExit`.

With this configuration, you should be able to start xfce normally
(e.g. from `~/.xinitrc`, from `~/.Xsession` or by graphically choosing
xfce session in a graphical login manager) and get a session, where
xfwm4 is replaced with i3.

If your setup defaults to Xfce without any `~/.xinitrc` or
`~/.Xsession`, then you are good to go.  We recommend to use
`~/.xsessionrc` for global session-wide environment variable setup.

Note, that this xml uses `xnilconslock` as locker, which is simply a
shell script to call `xsecurelock`, after setting it up to do DPMS and
kill scdaemon, so that any secrets stored in there are relocked:

```
#!/bin/bash

export XSECURELOCK_SHOW_DATETIME=1
export XSECURELOCK_BLANK_DPMS_STATE=off
export XSECURELOCK_BLANK_TIMEOUT=10

killall -9 scdaemon
xsecurelock
```

If you want to use `gpg-agent` as the ssh agent in your session (and
not run ssh agent at all), then add this XML snippet to `xfce4-session.xml`:

```
  <property name="startup" type="empty">
    <property name="ssh-agent" type="empty">
      <property name="type" type="string" value="gpg-agent"/>
      <property name="enabled" type="bool" value="false"/>
    </property>
    <property name="gpg-agent" type="empty">
      <property name="enabled" type="bool" value="false"/>
    </property>
  </property>
```

Using i3bar only for workspace switcher
---------------------------------------

There are various solutions to put i3 workspace info on a non-i3 bar:

  - plugin for xfce: https://github.com/denesb/xfce4-i3-workspaces-plugin
  - module for polybar: https://github.com/polybar/polybar/wiki/Module:-i3

All these options are unfortunately buggy and hard to configure.

So as an alternative, we provide a hacked version of `i3bar`, that has
two new options to position the workspace switcher part of it on top
of an already existing other bar (I personally use `xfce4-panel`).

Here is an example bar config for your `~/.i3/config` (the new parts
are the `unhide_width` and `unhide_left_margin` paramters, that you
can use to tune your i3bar positioning on top of the other panel):

```
bar {
    i3bar_command exec ~/nilcons-i3-xfce/build/i3bar
    position bottom
    mode hide
    hidden_state show
    tray_output none
    font pango:DejaVu Sans 8
    workspace_min_width 25
    unhide_width 400
    unhide_left_margin 26
    output primary
    modifier none
    colors {
        background #f6f5f4
        statusline #ffffff
        separator #666666

        focused_workspace  #ffffff #00ff00 #000000
        active_workspace   #333333 #447744 #000000
        inactive_workspace #000000 #999999 #000000
        urgent_workspace   #000000 #ff9999 #000000
    }
}
```

Please note, that to support the `unhide_*` new config options, you
also have to run the modified i3 wm from this repository, by changing
your `xfce4-session.xml` (sessions -> i3withxfce -> Client0_Command to
match your build path, e.g. `~/nilcons-i3-xfce/build/i3`).

Alternatively, if you want to use upstream i3 from your distribution
and only use our hacked i3bar, then you can configure the
`unhide_width` and `unhide_left_margin` options in the cmdline:

```
bar {
    i3bar_command exec ~/nilcons-i3-xfce/build/i3bar -m 40 -w 400
    ...
}
```

i3bar hack with multiple monitors and fullscreen
------------------------------------------------

The above hack unfortunately has a bug: since we are abusing hide mode
and that's implemented in i3bar with the override-redirect X11
protocol feature, the WM (i3) has no knowledge of the bar, and
therefore i3bar is showing on top of fullscreen windows...

To fix this, this patched i3bar is hiding itself if anybody goes
fullscreen, but unfortunately with the current i3bar source code
architecture, this is only easy to do on one monitor, so if you are
using 2 monitors, the recommendation is to run one i3bar on each, and
only run xfce4-panel on your main monitor (you can set xfce panel to
"primary").

```
bar {
    i3bar_command exec ~/nilcons-i3-xfce/build/i3bar -m 26 -w 400
    position bottom
    mode hide
    hidden_state show
    tray_output none
    font pango:DejaVu Sans 8
    workspace_min_width 25
    output primary
    modifier none
    colors {
        background #f6f5f4
        statusline #ffffff
        separator #666666

        focused_workspace  #ffffff #00ff00 #000000
        active_workspace   #333333 #447744 #000000
        inactive_workspace #000000 #999999 #000000
        urgent_workspace   #000000 #ff9999 #000000
    }
}

bar {
    i3bar_command exec ~/nilcons-i3-xfce/build/i3bar -m 26
    position bottom
    tray_output none
    font pango:DejaVu Sans 8
    output nonprimary
    modifier none
    colors {
        background #f6f5f4
        statusline #ffffff
        separator #666666

        focused_workspace  #ffffff #00ff00 #000000
        active_workspace   #333333 #447744 #000000
        inactive_workspace #000000 #999999 #000000
        urgent_workspace   #000000 #ff9999 #000000
    }
}
```

So, here you can see that we set up i3bar exactly as we discribed: the
first block is responsible for the main screen, where xfce4-panel is
also active, therefore we have to show ourselves only on top of it.
And the second block actives i3bar on all non-primary screens, your
second, third monitors.  On these screens, we are using normal dock
mode, because the xfce4-panel is only active on the primary screen,
therefore i3bar can use the whole width of the displays.

My recommended xfce4-panel plugins
----------------------------------

This is of course highly opinionated, but this should give you an idea
on how to set up your xfce4-panel.  These plugins I found stable and
well working even through multiple weeks of testing:

  - whisker menu (for starting apps the noob way)
  - separator (this is the placeholder for our i3bar workspace switcher)
  - status tray plugin
  - keyboard layouts
  - pulseaudio plugin (TODO: this causes the xfce4-panel.xml to change a lot, therefore not nice if the xml is in git)
  - power manager plugin
  - notification plugin
  - clipman
  - cpu graph
  - system load monitor
  - free space checker
  - screenshot
  - action buttons
  - clock

All these panel plugins have a lot of settings, that you should set up
once to match your liking.  The xml/text config of them is pretty
stable and easy to sync with git, located in `~/.config/xfce4`.

Recommended i3 shortcuts to work with the xfce4 environment
-----------------------------------------------------------

Also opinionated, but here are parts of my i3 config, that shows my
xfce specific keyboard shortcuts:

```
bindsym $mod+x exec exec xfce4-popup-whiskermenu -p
bindsym $mod+y exec exec xfce4-popup-clipman
bindsym $mod+p exec exec xfce4-display-settings --minimal
bindsym $mod+z exec exec xflock4

# Volume control
bindsym $mod+F10 exec --no-startup-id pactl set-sink-mute $(pactl get-default-sink) toggle
bindsym $mod+F11 exec --no-startup-id pactl set-sink-volume $(pactl get-default-sink) -1%
bindsym $mod+F12 exec --no-startup-id pactl set-sink-volume $(pactl get-default-sink) +1%
```

![Logo](docs/logo-30.png) i3: A tiling window manager
=====================================================

[![Build Status](https://github.com/i3/i3/actions/workflows/main.yml/badge.svg)](https://github.com/i3/i3/actions/workflows/main.yml)
[![Issue Stats](https://img.shields.io/github/issues/i3/i3.svg)](https://github.com/i3/i3/issues)
[![Pull Request Stats](https://img.shields.io/github/issues-pr/i3/i3.svg)](https://github.com/i3/i3/pulls)

[![Packages](https://repology.org/badge/latest-versions/i3.svg)](https://repology.org/metapackage/i3/versions)
[![Packages](https://repology.org/badge/tiny-repos/i3.svg)](https://repology.org/metapackage/i3/versions)

i3 is a tiling window manager for X11.

For more information about i3, please see [the project's website](https://i3wm.org/) and [online documentation](https://i3wm.org/docs/).

For information about contributing to i3, please see [CONTRIBUTING.md](.github/CONTRIBUTING.md).
