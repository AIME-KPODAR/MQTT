import paho.mqtt.client as mqtt

BROKER = "broker.hivemq.com"
TOPIC_MESURE = "monprojet123/capteur/valeur"
TOPIC_LED = "monprojet123/led/commande"

def on_connect(client, userdata, flags, reason_code, properties):
    print("Connecté au broker :", reason_code)
    client.subscribe(TOPIC_MESURE)

def on_message(client, userdata, msg):
    valeur = int(msg.payload.decode())
    print(f"Mesure reçue : {valeur}")
    client.publish(TOPIC_LED, "ON" if valeur > 2048 else "OFF")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, 1883)
client.loop_forever()