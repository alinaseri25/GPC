package org.verya.GPC

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.util.Log

class NotificationReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        when(intent.action) {
        "org.verya.GPC.ACTION_STOP" -> {
            TestBridge.nativeOnNotificationAction("stop")
        }
        "org.verya.GPC.ACTION_START" -> {
            TestBridge.nativeOnNotificationAction("start")
        }
    }
    }
}
