const express = require('express');
const path = require('path');
const PORT = process.env.PORT || 5000;

express()
    .use(express.static(path.join(__dirname, 'website')))
    .set('view engine', 'html')
    .get('/', (req, res) => res.render('index'))
    .listen(PORT, () => console.log(`Listening on ${PORT}`));

