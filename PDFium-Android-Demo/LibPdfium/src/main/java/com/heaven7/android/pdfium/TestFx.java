package com.heaven7.android.pdfium;

import android.graphics.Bitmap;

public final class TestFx {

    public static void draw(Bitmap bitmap){
        nDraw(bitmap);
    }
    public static void init(){
        nInit();
    }

    private static native void nInit();

    private static native void nDraw(Bitmap bitmap);
}
