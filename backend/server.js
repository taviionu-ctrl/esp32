const express = require("express");
const cors = require("cors");

const app = express();
app.use(cors());
app.use(express.json());

// ultima comandă (simplu, fără DB)
let desiredLed = 0; // 0=OFF, 1=ON

// SITE: apasă buton -> setează comanda
app.get("/api/led/on", (req, res) => {
  desiredLed = 1;
  res.status(204).end(); // fără feedback
});

app.get("/api/led/off", (req, res) => {
  desiredLed = 0;
  res.status(204).end();
});

// ESP32: citește comanda
app.get("/api/led/state", (req, res) => {
  res.json({ led: desiredLed });
});

const port = process.env.PORT || 3000;
app.listen(port, () => console.log("Listening on", port));
