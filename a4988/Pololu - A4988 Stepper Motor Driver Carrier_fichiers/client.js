!function(e){if("object"==typeof exports&&"undefined"!=typeof module)module.exports=e();else if("function"==typeof define&&define.amd)define([],e);else{var t;t="undefined"!=typeof window?window:"undefined"!=typeof global?global:"undefined"!=typeof self?self:this,(t.braintree||(t.braintree={})).client=e()}}(function(){return function(){function e(t,r,n){function o(a,s){if(!r[a]){if(!t[a]){var u="function"==typeof require&&require;if(!s&&u)return u(a,!0);if(i)return i(a,!0);var c=new Error("Cannot find module '"+a+"'");throw c.code="MODULE_NOT_FOUND",c}var d=r[a]={exports:{}};t[a][0].call(d.exports,function(e){return o(t[a][1][e]||e)},d,d.exports,e,t,r,n)}return r[a].exports}for(var i="function"==typeof require&&require,a=0;a<n.length;a++)o(n[a]);return o}return e}()({1:[function(e,t,r){(function(r){"use strict";var n=e("./is-ie11");t.exports=function(e){return e=e||r.navigator.userAgent,-1!==e.indexOf("MSIE")||n(e)}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{"./is-ie11":2}],2:[function(e,t,r){"use strict";t.exports=function(e){return e=e||navigator.userAgent,-1!==e.indexOf("Trident/7")}},{}],3:[function(e,t,r){"use strict";t.exports=function(e){return e=e||navigator.userAgent,-1!==e.indexOf("MSIE 9")}},{}],4:[function(e,t,r){"use strict";function n(e){return function(){var t=arguments;setTimeout(function(){e.apply(null,t)},1)}}t.exports=n},{}],5:[function(e,t,r){"use strict";function n(e){var t=!1;return function(){t||(t=!0,e.apply(null,arguments))}}t.exports=n},{}],6:[function(e,t,r){"use strict";function n(e,t){if(!t)return e;e.then(function(e){t(null,e)}).catch(function(e){t(e)})}t.exports=n},{}],7:[function(e,t,r){"use strict";function n(e){return function(){var t,r=Array.prototype.slice.call(arguments);return"function"==typeof r[r.length-1]&&(t=r.pop(),t=i(o(t))),a(e.apply(this,r),t)}}var o=e("./lib/deferred"),i=e("./lib/once"),a=e("./lib/promise-or-callback");n.wrapPrototype=function(e,t){var r,o,i;return t=t||{},o=t.ignoreMethods||[],i=!0===t.transformPrivateMethods,r=Object.getOwnPropertyNames(e.prototype).filter(function(t){var r,n="constructor"!==t&&"function"==typeof e.prototype[t],a=-1===o.indexOf(t);return r=!!i||"_"!==t.charAt(0),n&&r&&a}),r.forEach(function(t){var r=e.prototype[t];e.prototype[t]=n(r)}),e},t.exports=n},{"./lib/deferred":4,"./lib/once":5,"./lib/promise-or-callback":6}],8:[function(e,t,r){"use strict";function n(){}function o(e,t){return function(){e.apply(t,arguments)}}function i(e){if(!(this instanceof i))throw new TypeError("Promises must be constructed via new");if("function"!=typeof e)throw new TypeError("not a function");this._state=0,this._handled=!1,this._value=void 0,this._deferreds=[],p(e,this)}function a(e,t){for(;3===e._state;)e=e._value;if(0===e._state)return void e._deferreds.push(t);e._handled=!0,i._immediateFn(function(){var r=1===e._state?t.onFulfilled:t.onRejected;if(null===r)return void(1===e._state?s:u)(t.promise,e._value);var n;try{n=r(e._value)}catch(e){return void u(t.promise,e)}s(t.promise,n)})}function s(e,t){try{if(t===e)throw new TypeError("A promise cannot be resolved with itself.");if(t&&("object"==typeof t||"function"==typeof t)){var r=t.then;if(t instanceof i)return e._state=3,e._value=t,void c(e);if("function"==typeof r)return void p(o(r,t),e)}e._state=1,e._value=t,c(e)}catch(t){u(e,t)}}function u(e,t){e._state=2,e._value=t,c(e)}function c(e){2===e._state&&0===e._deferreds.length&&i._immediateFn(function(){e._handled||i._unhandledRejectionFn(e._value)});for(var t=0,r=e._deferreds.length;t<r;t++)a(e,e._deferreds[t]);e._deferreds=null}function d(e,t,r){this.onFulfilled="function"==typeof e?e:null,this.onRejected="function"==typeof t?t:null,this.promise=r}function p(e,t){var r=!1;try{e(function(e){r||(r=!0,s(t,e))},function(e){r||(r=!0,u(t,e))})}catch(e){if(r)return;r=!0,u(t,e)}}var l=setTimeout;i.prototype.catch=function(e){return this.then(null,e)},i.prototype.then=function(e,t){var r=new this.constructor(n);return a(this,new d(e,t,r)),r},i.all=function(e){return new i(function(t,r){function n(e,a){try{if(a&&("object"==typeof a||"function"==typeof a)){var s=a.then;if("function"==typeof s)return void s.call(a,function(t){n(e,t)},r)}o[e]=a,0==--i&&t(o)}catch(e){r(e)}}if(!e||void 0===e.length)throw new TypeError("Promise.all accepts an array");var o=Array.prototype.slice.call(e);if(0===o.length)return t([]);for(var i=o.length,a=0;a<o.length;a++)n(a,o[a])})},i.resolve=function(e){return e&&"object"==typeof e&&e.constructor===i?e:new i(function(t){t(e)})},i.reject=function(e){return new i(function(t,r){r(e)})},i.race=function(e){return new i(function(t,r){for(var n=0,o=e.length;n<o;n++)e[n].then(t,r)})},i._immediateFn="function"==typeof setImmediate&&function(e){setImmediate(e)}||function(e){l(e,0)},i._unhandledRejectionFn=function(e){"undefined"!=typeof console&&console&&console.warn("Possible Unhandled Promise Rejection:",e)},t.exports=i},{}],9:[function(e,t,r){"use strict";var n=e("@braintree/browser-detection/is-ie"),o=e("@braintree/browser-detection/is-ie9");t.exports={isIe:n,isIe9:o}},{"@braintree/browser-detection/is-ie":1,"@braintree/browser-detection/is-ie9":3}],10:[function(e,t,r){"use strict";function n(e){var t,r,n;if(e=e||{},t=JSON.stringify(e),!(r=e.gatewayConfiguration))throw new u(g.CLIENT_MISSING_GATEWAY_CONFIGURATION);if(["assetsUrl","clientApiUrl","configUrl"].forEach(function(e){if(e in r&&!s(r[e]))throw new u({type:g.CLIENT_GATEWAY_CONFIGURATION_INVALID_DOMAIN.type,code:g.CLIENT_GATEWAY_CONFIGURATION_INVALID_DOMAIN.code,message:e+" property is on an invalid domain."})}),this.getConfiguration=function(){return JSON.parse(t)},this._request=a,this._configuration=this.getConfiguration(),this._clientApiBaseUrl=r.clientApiUrl+"/v1/",(n=r.braintreeApi)&&(this._braintreeApi={baseUrl:n.url+"/",accessToken:n.accessToken},!s(this._braintreeApi.baseUrl)))throw new u({type:g.CLIENT_GATEWAY_CONFIGURATION_INVALID_DOMAIN.type,code:g.CLIENT_GATEWAY_CONFIGURATION_INVALID_DOMAIN.code,message:"braintreeApi URL is on an invalid domain."});r.graphQL&&(this._graphQL=new i({graphQL:r.graphQL}))}function o(e,t){var r;if(-1===e?r=new u(g.CLIENT_REQUEST_TIMEOUT):403===e?r=new u(g.CLIENT_AUTHORIZATION_INSUFFICIENT):429===e?r=new u(g.CLIENT_RATE_LIMITED):e>=500?r=new u(g.CLIENT_GATEWAY_NETWORK):(e<200||e>=400)&&(r=c(t,{type:g.CLIENT_REQUEST_ERROR.type,code:g.CLIENT_REQUEST_ERROR.code,message:g.CLIENT_REQUEST_ERROR.message})),r)return r.details=r.details||{},r.details.httpStatus=e,r}var i=e("./request/graphql"),a=e("./request"),s=e("../lib/is-whitelisted-domain"),u=e("../lib/braintree-error"),c=e("../lib/convert-to-braintree-error"),d=e("../lib/add-metadata"),p=e("../lib/promise"),l=e("@braintree/wrap-promise"),f=e("../lib/once"),h=e("../lib/deferred"),y=e("../lib/assign").assign,E=e("../lib/analytics"),I=e("./constants"),g=e("./errors"),_=e("../lib/errors"),A=e("../lib/constants").VERSION,T=e("../lib/methods"),m=e("../lib/convert-methods-to-error");n.prototype.request=function(e,t){var r=this,n=new p(function(t,n){var i,a,s,c;if(e.method?e.endpoint||(i="options.endpoint"):i="options.method",i)throw new u({type:g.CLIENT_OPTION_REQUIRED.type,code:g.CLIENT_OPTION_REQUIRED.code,message:i+" is required when making a request."});if(a="api"in e?e.api:"clientApi",c={method:e.method,graphQL:r._graphQL,timeout:e.timeout,metadata:r._configuration.analyticsMetadata},"clientApi"===a)s=r._clientApiBaseUrl,c.data=d(r._configuration,e.data);else{if("braintreeApi"!==a)throw new u({type:g.CLIENT_OPTION_INVALID.type,code:g.CLIENT_OPTION_INVALID.code,message:"options.api is invalid."});if(!r._braintreeApi)throw new u(_.BRAINTREE_API_ACCESS_RESTRICTED);s=r._braintreeApi.baseUrl,c.data=e.data,c.headers={"Braintree-Version":I.BRAINTREE_API_VERSION_HEADER,Authorization:"Bearer "+r._braintreeApi.accessToken}}c.url=s+e.endpoint,c.sendAnalyticsEvent=function(e){E.sendEvent(r,e)},r._request(c,function(e,r,i){var a,s;if(s=o(i,e))return void n(s);a=y({_httpStatus:i},r),t(a)})});return"function"==typeof t?(t=f(h(t)),void n.then(function(e){t(null,e,e._httpStatus)}).catch(function(e){var r=e&&e.details&&e.details.httpStatus;t(e,null,r)})):n},n.prototype.toJSON=function(){return this.getConfiguration()},n.prototype.getVersion=function(){return A},n.prototype.teardown=l(function(){return m(this,T(n.prototype)),p.resolve()}),t.exports=n},{"../lib/add-metadata":31,"../lib/analytics":32,"../lib/assign":33,"../lib/braintree-error":34,"../lib/constants":35,"../lib/convert-methods-to-error":36,"../lib/convert-to-braintree-error":37,"../lib/deferred":39,"../lib/errors":41,"../lib/is-whitelisted-domain":42,"../lib/methods":44,"../lib/once":45,"../lib/promise":46,"./constants":11,"./errors":12,"./request":25,"./request/graphql":23,"@braintree/wrap-promise":7}],11:[function(e,t,r){"use strict";t.exports={BRAINTREE_API_VERSION_HEADER:"2017-04-03"}},{}],12:[function(e,t,r){"use strict";var n=e("../lib/braintree-error");t.exports={CLIENT_GATEWAY_CONFIGURATION_INVALID_DOMAIN:{type:n.types.MERCHANT,code:"CLIENT_GATEWAY_CONFIGURATION_INVALID_DOMAIN"},CLIENT_OPTION_REQUIRED:{type:n.types.MERCHANT,code:"CLIENT_OPTION_REQUIRED"},CLIENT_OPTION_INVALID:{type:n.types.MERCHANT,code:"CLIENT_OPTION_INVALID"},CLIENT_MISSING_GATEWAY_CONFIGURATION:{type:n.types.INTERNAL,code:"CLIENT_MISSING_GATEWAY_CONFIGURATION",message:"Missing gatewayConfiguration."},CLIENT_INVALID_AUTHORIZATION:{type:n.types.MERCHANT,code:"CLIENT_INVALID_AUTHORIZATION",message:"Authorization is invalid. Make sure your client token or tokenization key is valid."},CLIENT_GATEWAY_NETWORK:{type:n.types.NETWORK,code:"CLIENT_GATEWAY_NETWORK",message:"Cannot contact the gateway at this time."},CLIENT_REQUEST_TIMEOUT:{type:n.types.NETWORK,code:"CLIENT_REQUEST_TIMEOUT",message:"Request timed out waiting for a reply."},CLIENT_REQUEST_ERROR:{type:n.types.NETWORK,code:"CLIENT_REQUEST_ERROR",message:"There was a problem with your request."},CLIENT_RATE_LIMITED:{type:n.types.MERCHANT,code:"CLIENT_RATE_LIMITED",message:"You are being rate-limited; please try again in a few minutes."},CLIENT_AUTHORIZATION_INSUFFICIENT:{type:n.types.MERCHANT,code:"CLIENT_AUTHORIZATION_INSUFFICIENT",message:"The authorization used has insufficient privileges."}}},{"../lib/braintree-error":34}],13:[function(e,t,r){(function(r){"use strict";function n(e){return new i(function(t,n){var i,a,f,h,y,E=u(),I={merchantAppId:r.location.host,platform:c.PLATFORM,sdkVersion:c.VERSION,source:c.SOURCE,integration:c.INTEGRATION,integrationType:c.INTEGRATION,sessionId:E};try{a=d(e.authorization)}catch(e){return void n(new o(p.CLIENT_INVALID_AUTHORIZATION))}f=a.attrs,h=a.configUrl,f._meta=I,f.braintreeLibraryVersion=c.BRAINTREE_LIBRARY_VERSION,f.configVersion="3",y={url:h,method:"GET",data:f},f.authorizationFingerprint&&a.graphQLUrl&&(y.graphQL=new l({graphQL:{url:a.graphQLUrl,features:["configuration"]}}),y.metadata=I),s(y,function(r,a,s){var u;if(r)return u=403===s?p.CLIENT_AUTHORIZATION_INSUFFICIENT:p.CLIENT_GATEWAY_NETWORK,void n(new o({type:u.type,code:u.code,message:u.message,details:{originalError:r}}));i={authorization:e.authorization,authorizationType:f.tokenizationKey?"TOKENIZATION_KEY":"CLIENT_TOKEN",analyticsMetadata:I,gatewayConfiguration:a},t(i)})})}var o=e("../lib/braintree-error"),i=e("../lib/promise"),a=e("@braintree/wrap-promise"),s=e("./request"),u=e("../lib/vendor/uuid"),c=e("../lib/constants"),d=e("../lib/create-authorization-data"),p=e("./errors"),l=e("./request/graphql");t.exports={getConfiguration:a(n)}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{"../lib/braintree-error":34,"../lib/constants":35,"../lib/create-authorization-data":38,"../lib/promise":46,"../lib/vendor/uuid":49,"./errors":12,"./request":25,"./request/graphql":23,"@braintree/wrap-promise":7}],14:[function(e,t,r){"use strict";function n(e){return e.authorization?p[e.authorization]?u.resolve(p[e.authorization]):s(e).then(function(t){var r;return e.debug&&(t.isDebug=!0),r=new a(t),p[e.authorization]=r,r}):u.reject(new i({type:d.INSTANTIATION_OPTION_REQUIRED.type,code:d.INSTANTIATION_OPTION_REQUIRED.code,message:"options.authorization is required when instantiating a client."}))}function o(){p={}}var i=e("../lib/braintree-error"),a=e("./client"),s=e("./get-configuration").getConfiguration,u=e("../lib/promise"),c=e("@braintree/wrap-promise"),d=e("../lib/errors"),p={};t.exports={create:c(n),VERSION:"3.33.0",_clearCache:o}},{"../lib/braintree-error":34,"../lib/errors":41,"../lib/promise":46,"./client":10,"./get-configuration":13,"@braintree/wrap-promise":7}],15:[function(e,t,r){"use strict";function n(e){return(!e||e===I)&&u.isIe()}function o(e){return"unknown_error"===(!e.data&&e.errors&&e.errors[0]&&e.errors[0].extensions&&e.errors[0].extensions.errorType)}function i(e,t,r){var a,u,I,g,_,A,T,m=e.url,N=e.graphQL,b=e.timeout,C=l.getRequestObject(),R=r,v=Boolean(N&&N.isGraphQLRequest(m,e.data));e.headers=c({"Content-Type":"application/json"},e.headers),I=v?new h(e):new y(e),m=I.getUrl(),g=I.getBody(),_=I.getMethod(),A=I.getHeaders(),"GET"===_&&(m=s.queryify(m,g),g=null),f?C.onreadystatechange=function(){if(4===C.readyState){if(0===C.status&&v)return delete e.graphQL,void i(e,t,r);if(T=p(C.responseText),u=I.adaptResponseBody(T),(a=I.determineStatus(C.status,T))>=400||a<200){if(v&&o(T))return delete e.graphQL,void i(e,t,r);if(t<E&&n(a))return t++,void i(e,t,r);R(u||"error",null,a||500)}else R(null,u,a)}}:(e.headers&&(m=s.queryify(m,A)),C.onload=function(){R(null,p(C.responseText),C.status)},C.onerror=function(){R("error",null,500)},C.onprogress=function(){},C.ontimeout=function(){R("timeout",null,-1)});try{C.open(_,m,!0)}catch(n){if(!v)throw n;return delete e.graphQL,void i(e,t,r)}C.timeout=b,f&&Object.keys(A).forEach(function(e){C.setRequestHeader(e,A[e])});try{C.send(d(_,g))}catch(e){}}function a(e,t){i(e,0,t)}var s=e("../../lib/querystring"),u=e("../browser-detection"),c=e("../../lib/assign").assign,d=e("./prep-body"),p=e("./parse-body"),l=e("./xhr"),f=l.isAvailable,h=e("./graphql/request"),y=e("./default-request"),E=1,I=408;t.exports={request:a}},{"../../lib/assign":33,"../../lib/querystring":47,"../browser-detection":9,"./default-request":16,"./graphql/request":24,"./parse-body":28,"./prep-body":29,"./xhr":30}],16:[function(e,t,r){"use strict";function n(e){this._url=e.url,this._data=e.data,this._method=e.method,this._headers=e.headers}n.prototype.getUrl=function(){return this._url},n.prototype.getBody=function(){return this._data},n.prototype.getMethod=function(){return this._method},n.prototype.getHeaders=function(){return this._headers},n.prototype.adaptResponseBody=function(e){return e},n.prototype.determineStatus=function(e){return e},t.exports=n},{}],17:[function(e,t,r){(function(e){"use strict";t.exports=function(){return e.navigator.userAgent}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{}],18:[function(e,t,r){"use strict";function n(e,t){return e.data&&!e.errors?o(e,t):a(e)}function o(e,t){var r,n=e.data.clientConfiguration;return r={environment:n.environment.toLowerCase(),clientApiUrl:n.clientApiUrl,assetsUrl:n.assetsUrl,analytics:{url:n.analyticsUrl},merchantId:n.merchantId,venmo:"off"},n.supportedFeatures&&(r.graphQL={url:t._graphQL._config.url,features:n.supportedFeatures.map(function(e){return e.toLowerCase()})}),n.braintreeApi&&(r.braintreeApi=n.braintreeApi),n.applePayWeb&&(r.applePayWeb=n.applePayWeb,r.applePayWeb.supportedCardBrands=i(n.applePayWeb.supportedCardBrands,u.applePayWeb)),n.ideal&&(r.ideal=n.ideal),n.kount&&(r.kount={kountMerchantId:n.kount.merchantId}),n.creditCard?(r.challenges=n.creditCard.challenges.map(function(e){return e.toLowerCase()}),r.creditCards={supportedCardTypes:i(n.creditCard.supportedCardBrands,u.creditCard)},r.threeDSecureEnabled=n.creditCard.threeDSecureEnabled):(r.challenges=[],r.creditCards={supportedCardTypes:[]},r.threeDSecureEnabled=!1),n.googlePay&&(r.androidPay={displayName:n.googlePay.displayName,enabled:!0,environment:n.googlePay.environment.toLowerCase(),googleAuthorizationFingerprint:n.googlePay.googleAuthorization,supportedNetworks:i(n.googlePay.supportedCardBrands,u.googlePay)}),n.venmo&&(r.payWithVenmo={merchantId:n.venmo.merchantId,accessToken:n.venmo.accessToken,environment:n.venmo.environment.toLowerCase()}),n.paypal?(r.paypalEnabled=!0,r.paypal=s({},n.paypal),r.paypal.currencyIsoCode=r.paypal.currencyCode,r.paypal.environment=r.paypal.environment.toLowerCase(),delete r.paypal.currencyCode):r.paypalEnabled=!1,n.unionPay&&(r.unionPay={enabled:!0,merchantAccountId:n.unionPay.merchantAccountId}),n.visaCheckout&&(r.visaCheckout={apikey:n.visaCheckout.apiKey,externalClientId:n.visaCheckout.externalClientId,supportedCardTypes:i(n.visaCheckout.supportedCardBrands,u.visaCheckout)}),n.masterpass&&(r.masterpass={merchantCheckoutId:n.masterpass.merchantCheckoutId,supportedNetworks:i(n.masterpass.supportedCardBrands,u.masterpass)}),n.usBankAccount&&(r.usBankAccount={routeId:n.usBankAccount.routeId,plaid:{publicKey:n.usBankAccount.plaidPublicKey}}),r}function i(e,t){return e.reduce(function(e,r){return t.hasOwnProperty(r)?e.concat(t[r]):e},[])}var a=e("./error"),s=e("../../../../lib/assign").assign,u={creditCard:{AMERICAN_EXPRESS:"American Express",DISCOVER:"Discover",INTERNATIONAL_MAESTRO:"Maestro",JCB:"JCB",MASTERCARD:"MasterCard",SOLO:"Solo",UK_MAESTRO:"UK Maestro",UNION_PAY:"UnionPay",VISA:"Visa"},applePayWeb:{VISA:"visa",MASTERCARD:"masterCard",DISCOVER:"discover",AMERICAN_EXPRESS:"amex"},visaCheckout:{VISA:"Visa",MASTERCARD:"MasterCard",DISCOVER:"Discover",AMERICAN_EXPRESS:"American Express"},googlePay:{VISA:"visa",MASTERCARD:"mastercard",DISCOVER:"discover",AMERICAN_EXPRESS:"amex"},masterpass:{VISA:"visa",MASTERCARD:"master",DISCOVER:"discover",AMERICAN_EXPRESS:"amex",DINERS:"diners",INTERNATIONAL_MAESTRO:"maestro",JCB:"jcb"}};t.exports=n},{"../../../../lib/assign":33,"./error":20}],19:[function(e,t,r){"use strict";function n(e){return e.data&&!e.errors?o(e):i(e)}function o(e){var t=e.data.tokenizeCreditCard,r=t.creditCard,n=r.last4?r.last4.substr(2,4):"",o=r.binData;return o&&(["commercial","debit","durbinRegulated","healthcare","payroll","prepaid"].forEach(function(e){o[e]?o[e]=s[o[e]]:o[e]="Unknown"}),["issuingBank","countryOfIssuance","productId"].forEach(function(e){o[e]||(o[e]="Unknown")})),{creditCards:[{binData:o,consumed:!1,description:n?"ending in "+n:"",nonce:t.token,details:{cardType:a[r.brandCode]||"Unknown",lastFour:r.last4||"",lastTwo:n},type:"CreditCard",threeDSecureInfo:null}]}}var i=e("./error"),a={AMERICAN_EXPRESS:"American Express",DINERS:"Discover",DISCOVER:"Discover",INTERNATIONAL_MAESTRO:"Maestro",JCB:"JCB",MASTERCARD:"MasterCard",UK_MAESTRO:"Maestro",UNION_PAY:"Union Pay",VISA:"Visa"},s={YES:"Yes",NO:"No",UNKNOWN:"Unknown"};t.exports=n},{"./error":20}],20:[function(e,t,r){"use strict";function n(e){var t=e.errors&&e.errors[0]&&e.errors[0].extensions&&e.errors[0].extensions.errorType;return"user_error"===t?i(e):t?o(e):{error:{message:"There was a problem serving your request"},fieldErrors:[]}}function o(e){return{error:{message:e.errors[0].message},fieldErrors:[]}}function i(e){var t=a(e.errors);return{error:{message:u(t)},fieldErrors:t}}function a(e){var t=[];return e.forEach(function(e){s(e.extensions.inputPath.slice(1),e,t)}),t}function s(e,t,r){var n,o=t.extensions.legacyCode,i=e[0];if(1===e.length)return void r.push({code:o,field:i,message:t.message});r.forEach(function(e){e.field===i&&(n=e)}),n||(n={field:i,fieldErrors:[]},r.push(n)),s(e.slice(1),t,n.fieldErrors)}function u(e){return{creditCard:"Credit card is invalid"}[e[0].field]}t.exports=n},{}],21:[function(e,t,r){"use strict";function n(){return{query:o,operationName:"ClientConfiguration"}}var o="query ClientConfiguration {   clientConfiguration {     analyticsUrl     environment     merchantId     assetsUrl     clientApiUrl     creditCard {       supportedCardBrands       challenges       threeDSecureEnabled     }     applePayWeb {       countryCode       currencyCode       merchantIdentifier       supportedCardBrands     }     googlePay {       displayName       supportedCardBrands       environment       googleAuthorization     }     ideal {       routeId       assetsUrl     }     kount {       merchantId     }     masterpass {       merchantCheckoutId       supportedCardBrands     }     paypal {       displayName       clientId       privacyUrl       userAgreementUrl       assetsUrl       environment       environmentNoNetwork       unvettedMerchant       braintreeClientId       billingAgreementsEnabled       merchantAccountId       currencyCode       payeeEmail     }     unionPay {       merchantAccountId     }     usBankAccount {       routeId       plaidPublicKey     }     venmo {       merchantId       accessToken       environment     }     visaCheckout {       apiKey       externalClientId       supportedCardBrands     }     braintreeApi {       accessToken       url     }     supportedFeatures   } }";t.exports=n},{}],22:[function(e,t,r){"use strict";function n(e){var t=e.creditCard,r=t&&t.billingAddress,n=t&&t.expirationDate,i=t&&(t.expirationMonth||n&&n.split("/")[0].trim()),a=t&&(t.expirationYear||n&&n.split("/")[1].trim()),s={input:{creditCard:{number:t&&t.number,expirationMonth:i,expirationYear:a,cvv:t&&t.cvv,cardholderName:t&&t.cardholderName},options:{}}};return r&&(s.input.creditCard.billingAddress=r),s.input=o(e,s.input),s}function o(e,t){var r;return e.creditCard&&e.creditCard.options&&"boolean"==typeof e.creditCard.options.validate?r=e.creditCard.options.validate:e.authorizationFingerprint&&e.tokenizationKey||e.authorizationFingerprint?r=!0:e.tokenizationKey&&(r=!1),"boolean"==typeof r&&(t.options=a({validate:r},t.options)),t}function i(e){return{query:s,variables:n(e),operationName:"TokenizeCreditCard"}}var a=e("../../../../lib/assign").assign,s="mutation TokenizeCreditCard($input: TokenizeCreditCardInput!) {   tokenizeCreditCard(input: $input) {     token     creditCard {       brandCode       last4       binData {         prepaid         healthcare         debit         durbinRegulated         commercial         payroll         issuingBank         countryOfIssuance         productId       }     }   } }";t.exports=i},{"../../../../lib/assign":33}],23:[function(e,t,r){"use strict";function n(e){this._config=e.graphQL}function o(e){return s.some(function(t){return void 0!==t.split(".").reduce(function(e,t){return e&&e[t]},e)})}var i=e("../../browser-detection"),a={tokenize_credit_cards:"payment_methods/credit_cards",configuration:"configuration"},s=["creditCard.options.unionPayEnrollment"];n.prototype.getGraphQLEndpoint=function(){return this._config.url},n.prototype.isGraphQLRequest=function(e,t){var r,n=this.getClientApiPath(e);return!(!this._isGraphQLEnabled()||!n||i.isIe9())&&(r=this._config.features.some(function(e){return a[e]===n}),!o(t)&&r)},n.prototype.getClientApiPath=function(e){var t,r=e.split("/client_api/v1/");return r.length>1&&(t=r[1].split("?")[0]),t},n.prototype._isGraphQLEnabled=function(){return Boolean(this._config)},t.exports=n},{"../../browser-detection":9}],24:[function(e,t,r){"use strict";function n(e){var t=e.graphQL.getClientApiPath(e.url);this._graphQL=e.graphQL,this._data=e.data,this._method=e.method,this._headers=e.headers,this._clientSdkMetadata={source:e.metadata.source,integration:e.metadata.integration,sessionId:e.metadata.sessionId},this._sendAnalyticsEvent=e.sendAnalyticsEvent||Function.prototype,this._generator=l[t],this._adapter=f[t],this._sendAnalyticsEvent("graphql.init")}function o(e,t){return!e&&t.errors[0].message}function i(e){return-1===e.indexOf("_")?e:e.toLowerCase().replace(/(\_\w)/g,function(e){return e[1].toUpperCase()})}function a(e){var t={};return Object.keys(e).forEach(function(r){var n=i(r);"object"==typeof e[r]?t[n]=a(e[r]):"number"==typeof e[r]?t[n]=String(e[r]):t[n]=e[r]}),t}var s=e("../../../lib/assign").assign,u=e("./generators/credit-card-tokenization"),c=e("./adapters/credit-card-tokenization"),d=e("./generators/configuration"),p=e("./adapters/configuration"),l={"payment_methods/credit_cards":u,configuration:d},f={"payment_methods/credit_cards":c,configuration:p};n.prototype.getUrl=function(){return this._graphQL.getGraphQLEndpoint()},n.prototype.getBody=function(){var e=a(this._data),t=this._generator(e),r=s({clientSdkMetadata:this._clientSdkMetadata},t);return JSON.stringify(r)},n.prototype.getMethod=function(){return"POST"},n.prototype.getHeaders=function(){var e,t;return this._data.authorizationFingerprint?(this._sendAnalyticsEvent("graphql.authorization-fingerprint"),e=this._data.authorizationFingerprint):(this._sendAnalyticsEvent("graphql.tokenization-key"),e=this._data.tokenizationKey),t={Authorization:"Bearer "+e,"Braintree-Version":"2018-05-04"},s({},this._headers,t)},n.prototype.adaptResponseBody=function(e){return this._adapter(e,this)},n.prototype.determineStatus=function(e,t){var r,n;return 200===e?(n=t.errors&&t.errors[0]&&t.errors[0].extensions&&t.errors[0].extensions.errorType,r=t.data&&!t.errors?200:"user_error"===n?422:"developer_error"===n?403:"unknown_error"===n?500:o(n,t)?403:500):r=e||500,this._sendAnalyticsEvent("graphql.status."+e),this._sendAnalyticsEvent("graphql.determinedStatus."+r),r},t.exports=n},{"../../../lib/assign":33,"./adapters/configuration":18,"./adapters/credit-card-tokenization":19,"./generators/configuration":21,"./generators/credit-card-tokenization":22}],25:[function(e,t,r){"use strict";function n(){return null==o&&(o=!(c()&&/MSIE\s(8|9)/.test(u()))),o}var o,i=e("../../lib/once"),a=e("./jsonp-driver"),s=e("./ajax-driver"),u=e("./get-user-agent"),c=e("./is-http");t.exports=function(e,t){t=i(t||Function.prototype),e.method=(e.method||"GET").toUpperCase(),e.timeout=null==e.timeout?6e4:e.timeout,e.data=e.data||{},n()?s.request(e,t):a.request(e,t)}},{"../../lib/once":45,"./ajax-driver":15,"./get-user-agent":17,"./is-http":26,"./jsonp-driver":27}],26:[function(e,t,r){(function(e){"use strict";t.exports=function(){return"http:"===e.location.protocol}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{}],27:[function(e,t,r){(function(r){"use strict";function n(e){e&&e.parentNode&&e.parentNode.removeChild(e)}function o(e,t){var n=document.createElement("script"),o=!1;return n.src=e,n.async=!0,n.onerror=function(){r[t]({message:"error",status:500})},n.onload=n.onreadystatechange=function(){o||this.readyState&&"loaded"!==this.readyState&&"complete"!==this.readyState||(o=!0,n.onload=n.onreadystatechange=null)},n}function i(e){try{delete r[e]}catch(t){r[e]=null}}function a(e,t){l[t]=setTimeout(function(){l[t]=null,r[t]({error:"timeout",status:-1}),r[t]=function(){i(t)}},e)}function s(e,t,o){r[o]=function(r){var a=r.status||500,s=null,u=null;delete r.status,a>=400||a<200?s=r:u=r,i(o),n(e),clearTimeout(l[o]),t(s,u,a)}}function u(e,t){var r,n="callback_json_"+d().replace(/-/g,""),i=e.url,u=e.data,l=e.method,f=e.timeout;i=p.queryify(i,u),i=p.queryify(i,{_method:l,callback:n}),r=o(i,n),s(r,t,n),a(f,n),c||(c=document.getElementsByTagName("head")[0]),c.appendChild(r)}var c,d=e("../../lib/vendor/uuid"),p=e("../../lib/querystring"),l={};t.exports={request:u}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{"../../lib/querystring":47,"../../lib/vendor/uuid":49}],28:[function(e,t,r){"use strict";t.exports=function(e){try{e=JSON.parse(e)}catch(e){}return e}},{}],29:[function(e,t,r){"use strict";t.exports=function(e,t){if("string"!=typeof e)throw new Error("Method must be a string");return"get"!==e.toLowerCase()&&null!=t&&(t="string"==typeof t?t:JSON.stringify(t)),t}},{}],30:[function(e,t,r){(function(e){"use strict";function r(){return n?new XMLHttpRequest:new XDomainRequest}var n=e.XMLHttpRequest&&"withCredentials"in new e.XMLHttpRequest;t.exports={isAvailable:n,getRequestObject:r}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{}],31:[function(e,t,r){"use strict";function n(e,t){var r,n=t?i(t):{},s=o(e.authorization).attrs,u=i(e.analyticsMetadata);n.braintreeLibraryVersion=a.BRAINTREE_LIBRARY_VERSION;for(r in n._meta)n._meta.hasOwnProperty(r)&&(u[r]=n._meta[r]);return n._meta=u,s.tokenizationKey?n.tokenizationKey=s.tokenizationKey:n.authorizationFingerprint=s.authorizationFingerprint,n}var o=e("./create-authorization-data"),i=e("./json-clone"),a=e("./constants");t.exports=n},{"./constants":35,"./create-authorization-data":38,"./json-clone":43}],32:[function(e,t,r){"use strict";function n(e){return Math.floor(e/1e3)}function o(e,t,r){var o=e.getConfiguration(),s=e._request,u=n(Date.now()),c=o.gatewayConfiguration.analytics.url,d={analytics:[{kind:i.ANALYTICS_PREFIX+t,timestamp:u}]};s({url:c,method:"post",data:a(o,d),timeout:i.ANALYTICS_REQUEST_TIMEOUT_MS},r)}var i=e("./constants"),a=e("./add-metadata");t.exports={sendEvent:o}},{"./add-metadata":31,"./constants":35}],33:[function(e,t,r){"use strict";function n(e){var t,r,n;for(t=1;t<arguments.length;t++){r=arguments[t];for(n in r)r.hasOwnProperty(n)&&(e[n]=r[n])}return e}var o="function"==typeof Object.assign?Object.assign:n;t.exports={assign:o,_assign:n}},{}],34:[function(e,t,r){"use strict";function n(e){if(!n.types.hasOwnProperty(e.type))throw new Error(e.type+" is not a valid type.");if(!e.code)throw new Error("Error code required.");if(!e.message)throw new Error("Error message required.");this.name="BraintreeError",this.code=e.code,this.message=e.message,this.type=e.type,this.details=e.details}var o=e("./enumerate");n.prototype=Object.create(Error.prototype),n.prototype.constructor=n,n.types=o(["CUSTOMER","MERCHANT","NETWORK","INTERNAL","UNKNOWN"]),n.findRootError=function(e){return e instanceof n&&e.details&&e.details.originalError?n.findRootError(e.details.originalError):e},t.exports=n},{"./enumerate":40}],35:[function(e,t,r){"use strict";t.exports={ANALYTICS_PREFIX:"web.",ANALYTICS_REQUEST_TIMEOUT_MS:2e3,INTEGRATION_TIMEOUT_MS:6e4,VERSION:"3.33.0",INTEGRATION:"custom",SOURCE:"client",PLATFORM:"web",BRAINTREE_LIBRARY_VERSION:"braintree/web/3.33.0"}},{}],36:[function(e,t,r){"use strict";var n=e("./braintree-error"),o=e("./errors");t.exports=function(e,t){t.forEach(function(t){e[t]=function(){throw new n({type:o.METHOD_CALLED_AFTER_TEARDOWN.type,code:o.METHOD_CALLED_AFTER_TEARDOWN.code,message:t+" cannot be called after teardown."})}})}},{"./braintree-error":34,"./errors":41}],37:[function(e,t,r){"use strict";function n(e,t){return e instanceof o?e:new o({type:t.type,code:t.code,message:t.message,details:{originalError:e}})}var o=e("./braintree-error");t.exports=n},{"./braintree-error":34}],38:[function(e,t,r){"use strict";function n(e){return/^[a-zA-Z0-9]+_[a-zA-Z0-9]+_[a-zA-Z0-9_]+$/.test(e)}function o(e){var t=e.split("_"),r=t[0];return{merchantId:t.slice(2).join("_"),environment:r}}function i(e){var t,r,i={attrs:{},configUrl:""};return n(e)?(r=o(e),i.attrs.tokenizationKey=e,i.configUrl=s[r.environment]+"/merchants/"+r.merchantId+"/client_api/v1/configuration"):(t=JSON.parse(a(e)),i.attrs.authorizationFingerprint=t.authorizationFingerprint,i.configUrl=t.configUrl,i.graphQLUrl=t.graphQLUrl),i}var a=e("../lib/vendor/polyfill").atob,s={production:"https://api.braintreegateway.com:443",sandbox:"https://api.sandbox.braintreegateway.com:443"};t.exports=i},{"../lib/vendor/polyfill":48}],39:[function(e,t,r){"use strict";t.exports=function(e){return function(){var t=arguments;setTimeout(function(){e.apply(null,t)},1)}}},{}],40:[function(e,t,r){"use strict";function n(e,t){return t=null==t?"":t,e.reduce(function(e,r){return e[r]=t+r,e},{})}t.exports=n},{}],41:[function(e,t,r){"use strict";var n=e("./braintree-error");t.exports={INVALID_USE_OF_INTERNAL_FUNCTION:{type:n.types.INTERNAL,code:"INVALID_USE_OF_INTERNAL_FUNCTION"},CALLBACK_REQUIRED:{type:n.types.MERCHANT,code:"CALLBACK_REQUIRED"},
INSTANTIATION_OPTION_REQUIRED:{type:n.types.MERCHANT,code:"INSTANTIATION_OPTION_REQUIRED"},INVALID_OPTION:{type:n.types.MERCHANT,code:"INVALID_OPTION"},INCOMPATIBLE_VERSIONS:{type:n.types.MERCHANT,code:"INCOMPATIBLE_VERSIONS"},METHOD_CALLED_AFTER_TEARDOWN:{type:n.types.MERCHANT,code:"METHOD_CALLED_AFTER_TEARDOWN"},BRAINTREE_API_ACCESS_RESTRICTED:{type:n.types.MERCHANT,code:"BRAINTREE_API_ACCESS_RESTRICTED",message:"Your access is restricted and cannot use this part of the Braintree API."}}},{"./braintree-error":34}],42:[function(e,t,r){"use strict";function n(e){return e.split(".").slice(-2).join(".")}function o(e){var t;return e=e.toLowerCase(),!!/^https:/.test(e)&&(i=i||document.createElement("a"),i.href=e,t=n(i.hostname),a.hasOwnProperty(t))}var i,a={"paypal.com":1,"braintreepayments.com":1,"braintreegateway.com":1,"braintree-api.com":1};t.exports=o},{}],43:[function(e,t,r){"use strict";t.exports=function(e){return JSON.parse(JSON.stringify(e))}},{}],44:[function(e,t,r){"use strict";t.exports=function(e){return Object.keys(e).filter(function(t){return"function"==typeof e[t]})}},{}],45:[function(e,t,r){arguments[4][5][0].apply(r,arguments)},{dup:5}],46:[function(e,t,r){(function(r){"use strict";var n=r.Promise||e("promise-polyfill");t.exports=n}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{"promise-polyfill":8}],47:[function(e,t,r){(function(e){"use strict";function r(e){var t;for(t in e)if(e.hasOwnProperty(t))return!0;return!1}function n(e){return e&&"object"==typeof e&&"number"==typeof e.length&&"[object Array]"===Object.prototype.toString.call(e)||!1}function o(t){var r;return t=t||e.location.href,/\?/.test(t)?(r=t.replace(/#.*$/,"").replace(/^.*\?/,"").split("&"),r.reduce(function(e,t){var r=t.split("="),n=decodeURIComponent(r[0]),o=decodeURIComponent(r[1]);return e[n]=o,e},{})):{}}function i(e,t){var r,o,a,s=[];for(a in e)e.hasOwnProperty(a)&&(o=e[a],r=t?n(e)?t+"[]":t+"["+a+"]":a,"object"==typeof o?s.push(i(o,r)):s.push(encodeURIComponent(r)+"="+encodeURIComponent(o)));return s.join("&")}function a(e,t){return e=e||"",null!=t&&"object"==typeof t&&r(t)&&(e+=-1===e.indexOf("?")?"?":"",e+=-1!==e.indexOf("=")?"&":"",e+=i(t)),e}t.exports={parse:o,stringify:i,queryify:a}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{}],48:[function(e,t,r){(function(e){"use strict";function r(e){var t,r,n,o,i,a,s,u,c=new RegExp("^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{4})([=]{1,2})?$"),d="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",p="";if(!c.test(e))throw new Error("Non base64 encoded input passed to window.atob polyfill");u=0;do{o=d.indexOf(e.charAt(u++)),i=d.indexOf(e.charAt(u++)),a=d.indexOf(e.charAt(u++)),s=d.indexOf(e.charAt(u++)),t=(63&o)<<2|i>>4&3,r=(15&i)<<4|a>>2&15,n=(3&a)<<6|63&s,p+=String.fromCharCode(t)+(r?String.fromCharCode(r):"")+(n?String.fromCharCode(n):"")}while(u<e.length);return p}var n="function"==typeof e.atob?e.atob:r;t.exports={atob:function(t){return n.call(e,t)},_atob:r}}).call(this,"undefined"!=typeof global?global:"undefined"!=typeof self?self:"undefined"!=typeof window?window:{})},{}],49:[function(e,t,r){"use strict";function n(){return"xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx".replace(/[xy]/g,function(e){var t=16*Math.random()|0;return("x"===e?t:3&t|8).toString(16)})}t.exports=n},{}]},{},[14])(14)});