QT.gui.VERSION = 5.15.1
QT.gui.name = QtGui
QT.gui.module = Qt5Gui
QT.gui.libs = $$QT_MODULE_LIB_BASE
QT.gui.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtGui
QT.gui.frameworks =
QT.gui.bins = $$QT_MODULE_BIN_BASE
QT.gui.plugin_types = accessiblebridge platforms platforms/darwin xcbglintegrations platformthemes platforminputcontexts generic iconengines imageformats egldeviceintegrations
QT.gui.depends = core
QT.gui.uses =
QT.gui.module_config = v2 staticlib
QT.gui.DEFINES = QT_GUI_LIB
QT.gui.enabled_features = accessibility action clipboard colornames cssparser cursor desktopservices imageformat_xpm draganddrop imageformatplugin highdpiscaling im image_heuristic_mask image_text imageformat_bmp imageformat_jpeg imageformat_png imageformat_ppm imageformat_xbm movie pdf picture sessionmanager shortcut standarditemmodel systemtrayicon tabletevent texthtmlparser textmarkdownreader textmarkdownwriter textodfwriter validator whatsthis wheelevent
QT.gui.disabled_features = opengles2 dynamicgl angle combined-angle-lib opengl openvg opengles3 opengles31 opengles32 system-textmarkdownreader vulkan
QT_DEFAULT_QPA_PLUGIN = qwindows
QT_CONFIG += accessibility action clipboard colornames cssparser cursor desktopservices imageformat_xpm draganddrop freetype fontconfig imageformatplugin harfbuzz highdpiscaling ico im image_heuristic_mask image_text imageformat_bmp imageformat_jpeg imageformat_png imageformat_ppm imageformat_xbm movie pdf picture sessionmanager shortcut standarditemmodel systemtrayicon tabletevent texthtmlparser textodfwriter validator whatsthis wheelevent
QT_MODULES += gui
