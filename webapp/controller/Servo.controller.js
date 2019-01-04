sap.ui.define([
	"sap/ui/core/mvc/Controller",
	"sap/ui/core/ws/WebSocket"
], function (Controller, WebSocket) {
	"use strict";

	return Controller.extend("de.linuxdozent.ServoControl.controller.Servo", {

		onInit: function () {
			// Check if WebSockets are supported
			if (!sap.ui.Device.support.websocket) {
				sap.m.MessageBox.show("Your SAPUI5 Version does not support WebSockets", {
					icon: sap.m.MessageBox.Icon.INFORMATION,
					title: "WebSockets not supported",
					actions: sap.m.MessageBox.Action.OK
				});
				return;
			}
			this.posBefore = 90;
			this.oModel = new sap.ui.model.json.JSONModel({
				"servo1pos": this.posBefore,
				"servo2pos": this.posBefore
			});
			this.getView().setModel(this.oModel);
			this.getWsConnection();
		},

		onSliderLiveChange: function (event) {
			var oSource = event.getSource();
			var p = parseInt(this.oModel.getProperty("/servo2pos").toString());
			var s = 2;
			var message = { p: p, s: s };
			this.oWs.send();
		},
		
		getWsConnection: function () {
			var wsServer = window.location.hostname;
			if(wsServer === 'localhost') {
				wsServer = "servoremote-188.local";
			}
			var wsURL = "ws://" + wsServer + "/ws";
			this.oWs = new WebSocket(wsURL);

			this.oWs.attachOpen(function (e) {
				sap.m.MessageToast.show('Websocket connection opened');
			});

			this.oWs.attachClose(function (e) {
				sap.m.MessageToast.show('Websocket connection closed');
				setTimeout(function () {
					this.getWsConnection();
				}.bind(this), 1000);
			}.bind(this));

			this.oWs.attachMessage(function (oEvent) {
				var message = oEvent.getParameter("data");
				this.oModel.setProperty("/pos", Number(message));
			}.bind(this));
		}
	});
});