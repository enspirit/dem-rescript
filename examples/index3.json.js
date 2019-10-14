function f(p1, p2) {
  var sum = p1+p2;
  var mul = p1*p2;
  var res = {executed: sum, executer: mul, data_format: p1-p2};
  return res;
};
JSON.stringify(f(1,2));
