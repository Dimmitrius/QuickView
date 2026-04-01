sed -i '/int CmsRenderingIntent = 1;/a\    int HdrToneMappingMode = 0;          // 0=Perceptual, 1=Colorimetric' QuickView/EditState.h
