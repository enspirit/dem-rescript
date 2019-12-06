'use strict'
let data = [{
  "foo": "bla",
  "bar" : "bli"
},
{
  "foo": "dlo",
  "bar" : "dlu"
}];

let promise = new Promise(function(resolve, reject) {
  resolve(data);
});

module.exports = promise;
