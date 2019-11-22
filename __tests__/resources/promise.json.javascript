'use strict'
let data = {
  "foo": "bla",
  "bar" : "bli"
};

let promise = new Promise(function(resolve, reject) {
  resolve(data);
});

module.exports = promise;
