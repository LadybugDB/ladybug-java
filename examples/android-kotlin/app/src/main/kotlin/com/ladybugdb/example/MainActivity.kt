package com.ladybugdb.example

import android.app.Activity
import android.os.Bundle
import android.widget.TextView
import com.ladybugdb.Database
import com.ladybugdb.SystemConfig

class MainActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val config = exampleConfig()
        setContentView(TextView(this).apply {
            text = "LadybugDB Kotlin example: ${config.maxNumThreads} threads"
        })
    }
}

fun exampleConfig(): SystemConfig = SystemConfig().apply {
    maxNumThreads = 2
    enableDefaultHashIndex = true
}

fun openDatabase(path: String, config: SystemConfig): Database = Database(path, config)
