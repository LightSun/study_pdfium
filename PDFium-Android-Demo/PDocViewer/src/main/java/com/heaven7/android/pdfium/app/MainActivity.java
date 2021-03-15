package com.heaven7.android.pdfium.app;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.heaven7.android.pdfium.TestFx;
import com.knizha.PDocViewer.PDocMainActivity;
import com.knziha.PDocViewer.R;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ac_main);
    }

    public void onClickTestFx(View view) {
        startActivity(new Intent(this, TestFxAc.class));
    }

    public void onClickTestPdf(View view) {
        startActivity(new Intent(this, PDocMainActivity.class));
    }
}
