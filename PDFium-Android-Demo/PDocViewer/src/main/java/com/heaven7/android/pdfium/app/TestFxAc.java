package com.heaven7.android.pdfium.app;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.heaven7.android.pdfium.TestFx;
import com.knziha.PDocViewer.R;

public class TestFxAc extends AppCompatActivity {

    static {
        System.loadLibrary("pdfium-lib");
        TestFx.init();
    }

    ImageView mIv;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ac_test_fx);
        mIv = findViewById(R.id.iv);
    }

    public void onClickTestBase(View view) {
        Bitmap bitmap = Bitmap.createBitmap(mIv.getWidth(), mIv.getHeight(), Bitmap.Config.ARGB_8888);
        TestFx.draw(bitmap);
        mIv.setImageBitmap(bitmap);
    }
}
