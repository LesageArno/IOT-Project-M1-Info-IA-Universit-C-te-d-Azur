from flask import Flask, request
app = Flask(__name__)

@app.route("/", methods=["POST"])
def receive():
    print("Received POST data:", request.get_json())
    return "OK", 200

app.run(host="0.0.0.0", port=5000)