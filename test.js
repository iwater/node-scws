var scws = require("./build/Release/nscws.node");
var worker = new scws.Scws();
worker.setDict(__dirname + "/stock_dict.xdb");
var a = worker.serialize("路翔股份第四届董事会第三十八次会议于2013年9月9日召开，审议通过了《关于拟向中国银行广州东山支行申请人民币2.8亿元综合授信额度的议案》。");
console.log(a);
var worker2 = new scws.Scws();
worker2.setDict("/opt/liujun/DataMining/word_server/dicts/main.xdb");
var b = worker2.serialize("路翔股份第四届董事会第三十八次会议于2013年9月9日召开，审议通过了《关于拟向中国银行广州东山支行申请人民币2.8亿元综合授信额度的议案》。");
console.log(b);
//module.exports = scws;
