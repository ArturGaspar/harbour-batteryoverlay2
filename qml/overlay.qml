import QtQuick 2.1
import Sailfish.Silica 1.0
import QtSensors 5.0
import Nemo.Configuration 1.0
import org.nemomobile.systemsettings 1.0

Item {
    id: root

    width: Screen.width
    height: Screen.height
    visible: battery.chargePercentage <= configuration.threshold

    Item {
        id: rotationItem

        anchors.centerIn: root
        width: orientationSensor.angle % 180 == 0 ? Screen.width : Screen.height
        height: orientationSensor.angle % 180 == 0 ? Screen.height : Screen.width
        rotation: orientationSensor.angle

        opacity: configuration.opacityPercentage / 100.0

        property bool inverted: !configuration.followOrientation && (configuration.fixedOrientation == 1
                                                                     || configuration.fixedOrientation == 2)

        Rectangle {
            id: chargedBar
            x: rotationItem.inverted ? unchargedBar.width : 0
            height: configuration.lineHeight
            width: rotationItem.width * battery.chargePercentage / 100
            gradient: Gradient {
                GradientStop { position: 0.0; color: configuration.chargedColor }
                GradientStop { position: 1.0; color: configuration.gradientOpacity ? "transparent" : configuration.chargedColor }
            }
        }

        Rectangle {
            id: unchargedBar
            x: rotationItem.inverted ? 0 : chargedBar.width
            width: rotationItem.width - chargedBar.width
            height: configuration.lineHeight
            gradient: Gradient {
                GradientStop { position: 0.0; color: configuration.unchargedColor }
                GradientStop { position: 1.0; color: configuration.gradientOpacity ? "transparent" : configuration.unchargedColor }
            }
        }
    }

    OrientationSensor {
        id: orientationSensor
        active: configuration.followOrientation
        property var hack: if (reading && reading.orientation) _getOrientation(reading.orientation)
        property int sensorAngle: 0
        property int angle: active
                              ? (configuration.orientationLock == "dynamic" || configuration.orientationLock == ""
                                 ? sensorAngle
                                 : (configuration.orientationLock == "portrait" ? 0 : 90))
                              : (configuration.fixedOrientation * 90)
        function _getOrientation(value) {
            switch (value) {
            case 1:
                sensorAngle = 0
                break
            case 2:
                sensorAngle = 180
                break
            case 3:
                sensorAngle = -90
                break
            case 4:
                sensorAngle = 90
                break
            default:
                return false
            }
            return true
        }
    }

    QtObject {
        id: battery
        property int chargePercentage: batteryStatus.chargePercentage
        property bool isCharging: batteryStatus.chargerStatus === BatteryStatus.Connected
    }

    BatteryStatus {
        id: batteryStatus
    }

    ConfigurationGroup {
        id: internal
        path: "/apps/harbour-battery-overlay"
        property bool followOrientation: false
        property int lineHeight: 5
        property int opacityPercentage: 50
        property string normalChargedColor: "green"
        property string normalUnchangedColor: "red"
        property string chargingChargedColor: "cyan"
        property string chargingUnchargedColor: "blue"
        property bool useSystemColors: false
        property bool displayChargingStatus: false
        property int fixedOrientation: 0
        property bool gradientOpacity: true
        property int threshold: 100
    }

    ConfigurationValue {
        id: orientationConf
        key: "/lipstick/orientationLock"
        defaultValue: "dynamic"
    }

    QtObject {
        id: configuration

        property bool followOrientation: internal ? internal.followOrientation : false
        property int lineHeight: internal ? internal.lineHeight : 5
        property int opacityPercentage: internal ? internal.opacityPercentage : 50
        property string normalChargedColor: internal ? internal.normalChargedColor : "green"
        property string normalUnchangedColor: internal ? internal.normalUnchangedColor : "red"
        property string chargingChargedColor: internal ? internal.chargingChargedColor : "cyan"
        property string chargingUnchargedColor: internal ? internal.chargingUnchargedColor : "blue"
        property bool useSystemColors: internal ? internal.useSystemColors : false
        property bool displayChargingStatus: internal ? internal.displayChargingStatus : false
        property int fixedOrientation: internal ? internal.fixedOrientation : 0
        property bool gradientOpacity: internal ? internal.gradientOpacity : true
        property int threshold: internal ? internal.threshold : 100

        property string systemChargedColor: displayChargingStatus && battery.isCharging
                                            ? Theme.highlightColor
                                            : Theme.highlightBackgroundColor
        property string systemUnchargedColor: displayChargingStatus && battery.isCharging
                                              ? Theme.secondaryHighlightColor
                                              : Theme.highlightDimmerColor
        property string settingsChargedColor: displayChargingStatus && battery.isCharging
                                              ? chargingChargedColor
                                              : normalChargedColor
        property string settingsUnchargedColor: displayChargingStatus && battery.isCharging
                                                ? chargingUnchargedColor
                                                : normalUnchangedColor
        property string chargedColor: useSystemColors ? systemChargedColor : settingsChargedColor
        property string unchargedColor: useSystemColors ? systemUnchargedColor : settingsUnchargedColor

        property string orientationLock: orientationConf ? orientationConf.value : "dynamic"
    }
}
