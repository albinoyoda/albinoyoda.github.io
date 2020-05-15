/* eslint-disable @typescript-eslint/no-var-requires */

const express = require('express')
const path = require('path')
const app = express()
const port = 1337

app.use(express.static(path.join(__dirname, '')))

app.listen(port, () => console.log(`app listening to http://localhost:${port}`))
