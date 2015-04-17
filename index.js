var scws = require("./build/Release/nscws.node");
module.exports = new scws.Scws();
module.exports.createWorker = function () {
  return new scws.Scws();
};
