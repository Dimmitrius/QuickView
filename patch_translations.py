import re

with open('QuickView/AppStrings.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# Rename Settings_Label_ResizeOnZoom to Settings_Label_LockWindow globally
content = content.replace('Settings_Label_ResizeOnZoom', 'Settings_Label_LockWindow')

# Update specific translations for LockWindow
content = content.replace('L"Resize on Zoom"', 'L"Lock Window Size"')
content = content.replace('L"缩放时调整窗口"', 'L"锁定窗口"')
content = content.replace('L"縮放時調整視窗"', 'L"鎖定視窗"')
content = content.replace('L"ズーム時にリサイズ"', 'L"ウィンドウをロック"')
content = content.replace('L"Изменять размер при масштабировании"', 'L"Заблокировать размер окна"')
content = content.replace('L"Größe bei Zoom ändern"', 'L"Fenstergröße sperren"')
content = content.replace('L"Redimensionar al hacer zoom"', 'L"Bloquear ventana"')

# Shorten UpscaleSmallImagesWhenLocked strings to prevent wrapping
content = content.replace('L"Adapt small images to window size"', 'L"Adapt small images"')
content = content.replace('L"小于窗口尺寸的图片适应窗口尺寸显示"', 'L"小于窗口尺寸图片适应窗口"')
content = content.replace('L"小於視窗尺寸的圖片適應視窗尺寸顯示"', 'L"小於視窗尺寸圖片適應視窗"')
content = content.replace('L"小さな画像をウィンドウサイズに合わせる"', 'L"小さな画像をウィンドウに合わせる"')
content = content.replace('L"Увеличивать маленькие изображения до размера окна"', 'L"Адаптировать мелкие изображения"')
content = content.replace('L"Kleine Bilder an Fenstergröße anpassen"', 'L"Kleine Bilder anpassen"')
content = content.replace('L"Adaptar imágenes pequeñas al tamaño de la ventana"', 'L"Adaptar imágenes pequeñas"')

with open('QuickView/AppStrings.cpp', 'w', encoding='utf-8') as f:
    f.write(content)
