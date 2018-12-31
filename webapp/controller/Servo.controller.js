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
				"pos": this.posBefore,
				"buttonPos": 80
			});
			this.getView().setModel(this.oModel);
			this.getWsConnection();
		},

		onSliderLiveChange: function (event) {
			this.oWs.send(this.oModel.getProperty("/pos").toString());
		},
		
		getWsConnection: function () {
			this.oWs = new WebSocket(
				"ws://servoremote-188.local/ws"
			);

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