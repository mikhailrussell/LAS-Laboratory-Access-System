var tid,xhr,j$=jQuery.noConflict(),cartCookieRequested=!1;!function(e,t){var n,a=e.jQuery||e.Cowboy||(e.Cowboy={});a.throttle=n=function(e,n,o,i){var r,s=0;function l(){var a=this,l=+new Date-s,c=arguments;function d(){s=+new Date,o.apply(a,c)}i&&!r&&d(),r&&clearTimeout(r),i===t&&l>e?d():!0!==n&&(r=setTimeout(i?function(){r=t}:d,i===t?e-l:e))}return"boolean"!=typeof n&&(i=o,o=n,n=t),a.guid&&(l.guid=o.guid=o.guid||a.guid++),l},a.debounce=function(e,a,o){return o===t?n(e,a,!1):n(e,o,!1!==a)}}(this);var Xplore=Xplore||{};function acmPopup(){var e=j$(this).attr("data-doi"),t=screen.width/2-300,n=screen.height/2-200,a="https://doi.org/"+e;return window.open(a,"Rightslink","location=no,toolbar=no,directories=no,status=no, menubar=no,scrollbars=yes,resizable=yes,top="+n+"px,left="+t+"px,width=600px,height=400px"),!1}function loadSSOJs(){if(IBP_WS_ENABLED_FLAG)for(var e=IBP_WS_ASSETS+"/ieee-mashup/js",t=["/common/jquery.json-2.2.min.js","/common/jquery.cookie-min.js","/auth/ieee-auth-constants-min.js","/auth/ieee-auth-include-min.js","/common/postmessage-min.js"],n=0;n<t.length;n++)j$.getScript(e+t[n],(function(){}))}function changeTypeToPassword(e,t){j$("#"+e+"password-txt-span").hide(),j$("#"+e+"password-hidden-span").show(),j$("#"+t).focus()}function changeTypeToText(e,t){j$.trim(j$("#"+t).val()).length<=0&&(j$("#"+e+"password-txt-span").show(),j$("#"+e+"password-hidden-span").hide())}function redirectToWayf(){var e;"0"==(e=j$.urlParam("url"))&&(e=null==window.parent?window.location.href:window.parent.location.href),e.indexOf("guesthome.jsp")>=0&&(e="/Xplore/home.jsp"),e=j$.URLEncode(e),null==window.parent?window.location.href="/servlet/wayf.jsp?url="+e:window.parent.location.href="/servlet/wayf.jsp?url="+e}function attemptMergeCart(){if(!cartCookieRequested&&IS_INDIVIDUAL_USER){var e=0;abortTimer();j$.ajax({url:"/xpl/mwGetIeeeUserInfo.jsp",method:"POST",cache:!1,timeout:28e3,async:!1,data:{}}).responseText;cartCookie=j$.cookie("ieeeUserInfoCookie"),console.log("cartCookie after merge: "+cartCookie),null!=cartCookie&&cartCookie.length>0?(cartCookieObj=j$.parseJSON(cartCookie),cartCookieObj.cartItemQty&&(e=cartCookieObj.cartItemQty),e>0?j$("#cartCount").html("Cart&nbsp;("+e+")"):j$("#cartCount").html("Cart&nbsp;"),1==isFunction("mc_forceRefreshMiniCart")&&mc_forceRefreshMiniCart()):j$("#cartCount").html("Cart&nbsp;"),cartCookieRequested=!0}else j$("#cartCount").html("Cart&nbsp;")}function abortTimer(){clearInterval(tid)}function cartBoxCheck(){var e=j$(".mc-product-cart");e&&(e.find(".mc-header").length?e.hasClass("box")||e.addClass("box box-style-4"):e.removeClass("box box-style-4"))}function stopCrossSiteScripting(e){return e}function setCurrentCartCount(){var e=0;if(cartBoxCheck(),null!=j$&&null!=j$.cookie)if(cartCookie=j$.cookie("ieeeUserInfoCookie"),cartCookie){-1==cartCookie.indexOf("userInfoId")&&attemptMergeCart();var t=j$.parseJSON(cartCookie);t.cartItemQty&&(e=t.cartItemQty),e>0?j$("#cartCount").html("Cart&nbsp;("+e+")"):j$("#cartCount").html("Cart&nbsp;")}else attemptMergeCart()}function addedToCartText(e){j$("#addedToCartSpanBundle").show(1e3,(function(){"Bundle"===e?j$("#addedToCartSpanBundle").text("Bundled item is added to cart. To purchase individual item, remove bundle from cart."):j$("#addedToCartSpanBundle").text("Individual item is added to cart. To purchase bundle, remove individual item from cart.")})),j$("#addToCartSpan").find("#add-to-cart-button").attr({src:ASSETS_RELATIVE_PATH+"/img/btn.add-to-cart-g.png?cv="+ASSETS_VERSION,disabled:"disabled"})}function repaintWrapper(){j$("#flt-container").length&&(j$(".mc-header").length?j$("#flt-container").ibpFloatVertical({bottomCollisionId:"FooterWrapper"}):j$("#flt-container").unbind("cartScroll").attr("style",""))}Xplore.config={THROTTLE_DELAY:1e3,DEBOUNCE_THRESHOLD:1e3},j$(document).ready((function(){j$(".acmPopUp").bind("click",acmPopup),jQuery.validator.addMethod("multiemails",(function(e,t){if(this.optional(t))return!0;var n=e.split(/[;,]+/);for(valid=!0,i=0;i<n.length;i++)e=n[i],valid=valid&&jQuery.validator.methods.email.call(this,j$.trim(e),t);return valid}),jQuery.validator.messages.multiemails),j$.fn.doOnce=function(e){return this.length&&e.apply(this),this},jQuery.fn.placeholder.input||j$("label.overlabel").overlabel(),j$("#LayoutWrapper").delegate("#singleSignOnClose","click",(function(e){e.preventDefault(),j$(this).closest("#singleSignOnFlyout").hide()})),j$("#LayoutWrapper").delegate("#singleSignOnClose","keypress",(function(e){13==e.keyCode&&(j$(this).closest("#singleSignOnFlyout").hide(),j$("#singleSignOnLink").focus())})),j$("#singleSignOnLink").click((function(){return loadSSOJs(),j$("#modalWindowSignInError281").hide(),j$("#loadingImg").hide(),j$("#singleSignOnFlyout").slideDown(0),j$("#singleSignOnLink2").focus(),!1})),j$("#AuthTools .SubMenu a").width(j$("#SignOutFlyOutLink").width()-20>90?j$("#SignOutFlyOutLink").width()-20:90),j$.support.touchEvents="ontouchstart"in window||window.DocumentTouch&&document instanceof DocumentTouch,j$("div.select-all-checkboxes").doOnce((function(){this.allCheckboxes()})),j$("#UtilityNav").doOnce((function(){this.dropdown({hclass:"Hover",el:".DHTMLMenu > *"})})),j$("#ToolBar").doOnce((function(){this.dropdown({hclass:"Hover",el:".DHTMLMenu > *"})})),j$("#toolbarSearchbar").doOnce((function(){this.dropdown({hclass:"hover",el:".DHTMLMenu > *"})})),j$("#popup-search-preferences-mysettings").delegate(j$(this),"click",(function(){j$("#popup-search-preferences").trigger("click")})),IBP_WS_ENABLED_FLAG&&setCurrentCartCount()})),function(e){e.fn.dropdown=function(t){defaults={hclass:"hover",el:"li"};t=e.extend(defaults,t);return this.each((function(){var n=e(this).find(t.el).siblings().andSelf();n.each((function(){e(this).data("open",!1)}));var a=function(){n.each((function(){1==e(this).data("open")&&e(this).removeClass(t.hclass).data("open",!1)})),e(this).addClass(t.hclass).data("open",!0)},o=function(){1==e(this).data("open")&&e(this).removeClass(t.hclass).data("open",!1)};if(e.fn.hoverIntent){var i={over:a,timeout:500,out:o};n.hoverIntent(i)}else n.hover(a,o)}))}}(jQuery),function(e){e.fn.tabify=function(t){defaults={target_el:"div.section"};t=e.extend(defaults,t);return this.each((function(){var n=e(this),a=n.find("div.nav-tabs"),o=a.find("li"),i=n.find(t.target_el);o.eq(0).addClass("active"),i.hide(),i.eq(0).show(),a.delegate("a","click",(function(t){t.preventDefault();var n=e(this),a=n.attr("href");i.hide(),e(a).show(),o.removeClass("active"),n.closest("li").addClass("active")}))}))}}(jQuery),function(e){e.fn.carousel=function(t){defaults={speed:500,indicate:!1,autoplay:!1,delay:1e4};t=e.extend(defaults,t);return this.each((function(){var n,a=e(this),o=a.find("div.wrapper"),i=o.find("div.slider"),r=i.find("div.item"),s=r.eq(0).outerWidth(),l=Math.ceil(o.innerWidth()/s),c=1,d=Math.ceil(r.length/l);if(r.length<=l)return!1;if(r.length%l){for(var u=l-r.length%l,f=0;f<u;f++)i.append('<div class="item empty" />');r=i.find("div.item")}function p(e){var a=e<c?-1:1,i=Math.abs(c-e),r=s*a*l*i;o.filter(":not(:animated)").animate({scrollLeft:"+="+r},t.speed,(function(){var a;0==e?(o.scrollLeft(s*l*d),e=d):e>d&&(o.scrollLeft(s*l),e=1),c=e,t.indicate&&(a=e,n.find("span.active").removeClass("active"),n.find("span").eq(a-1).addClass("active"))}))}r.filter(":first").before(r.slice(-l).clone().addClass("clone")),r.filter(":last").after(r.slice(0,l).clone().addClass("clone")),r=i.find("div.item"),o.scrollLeft(s*l);var h=e('<div class="controls" />'),m=(e('<span class="button prev" />').on("click",(function(){p(c-1)})).appendTo(h),e('<span class="button next" />').on("click",(function(){p(c+1)})).appendTo(h));if(h.appendTo(a),t.indicate){n=e('<div class="indicators" />');for(f=1;f<=d;f++)e("<span>"+f+"</span>").on("click",(function(){p(e(this).data("ref"))})).data("ref",f).appendTo(n);n.find("span").eq(0).addClass("active"),n.appendTo(a)}t.autoplay&&e(window).load((function(){var e=!0;a.hover((function(){e=!1}),(function(){e=!0})),setInterval((function(){e&&m.trigger("click")}),t.delay)}))}))}}(jQuery),function(e){e.fn.allCheckboxes=function(t){defaults={parent_el:"#results-blk"};t=e.extend(defaults,t);return this.each((function(){var n=e(this),a=e(this).find(":checkbox.all"),o=!1;a.prop("checked",!1),n.show(),$targets=a.parents(t.parent_el).find(":checkbox").not(a),a.on("click",(function(){$targets.prop("checked",this.checked),a.prop("checked")&&(o=!0)})),$targets.on("click",(function(){o&&(a.prop("checked",!1),o=!1)}))}))}}(jQuery),j$.urlParam=function(e){var t=new RegExp("[\\?&]"+e+"=([^&#]*)").exec(window.location.href);return t&&decodeURIComponent(t[1].replace(/\+/g," "))||0},j$.extend({URLEncode:function(e){var t="",n=0;e=e.toString();for(var a=/(^[a-zA-Z0-9_.]*)/;n<e.length;){var o=a.exec(e.substr(n));if(null!=o&&o.length>1&&""!=o[1])t+=o[1],n+=o[1].length;else{if(" "==e[n])t+="+";else{var i=e.charCodeAt(n).toString(16);t+="%"+(i.length<2?"0":"")+i.toUpperCase()}n++}}return t},URLDecode:function(e){for(var t,n=e,a=/(%[^%]{2})/;null!=(m=a.exec(n))&&m.length>1&&""!=m[1];)b=parseInt(m[1].substr(1),16),t=String.fromCharCode(b),n=n.replace(m[1],t);return n}}),j$("li.tl-print").on("click",(function(){j$(this).hasClass("disabled")||window.print()})),function(e){e.fn.overlabel=function(t){var n=e.extend({},e.fn.overlabel.defaults,t),a=this.filter("label[for]").map((function(){var t=e(this);t.css("display","inline");var a=t.attr("for"),o=t.siblings("#"+a);if(o){var i=e.meta?e.extend({},n,t.data()):n;t.addClass(i.label_class);var r=function(){t.css(i.hide_css)},s=function(){this.value||t.css(i.show_css)};return e(o).parent().addClass(i.wrapper_class).end().focus(r).blur(s).each(r).each(s),this}}));return n.filter?a:a.end()},e.fn.overlabel.defaults={label_class:"overlabel-apply",wrapper_class:"overlabel-wrapper",hide_css:{"text-indent":"-10000px"},show_css:{"text-indent":"0px",cursor:"text"},filter:!1}}(jQuery),function(e){e.fn.actionBar=function(t){defaults={layout:"horizontal"};t=e.extend(defaults,t);return this.each((function(){var n=e(this),a=e(window),o=e(document).height(),i=e('<div id="modal-mask" />');n.bind("click",(function(r){if(r.preventDefault(),n.parent().hasClass("disabled"))return!1;n.focus();var s=e("#pop-container"),l=function(){e("#modal-mask").remove(),s.empty()};if(e("#modal-mask").length)return l(),!1;if(!s.length){s=e('<div id="pop-container" />').appendTo(n.parents("div.actionbar"));e(document).on("click","span.close-popup",(function(){l()}))}"relative-to-object"==t.layout?s.css({marginLeft:n.parent().width(),marginTop:-n.position().top}):"vertical"==t.layout?s.css({marginTop:n.parent().position().top}):s.css({marginLeft:n.position().left});var c=n.attr("href"),d=t.actionId;s.loadUrl({hrefURL:c,actionId:d}),i.on("click",(function(){l()})).css({width:a.width(),height:o}).appendTo(e("body"))}))}))}}(jQuery),j$((function(){j$(".mw-contAdmin").qtip({content:{text:j$("#contAdminWindowContent")},show:{event:"click"},hide:{fixed:!0,event:"unfocus"},position:{my:"top left",at:"bottom left",adjust:{x:2,y:2}},style:{width:"auto",classes:"qtip-lightIeee",tip:!1},events:{render:function(e,t){var n=j$(this);n.delegate(".mwclose","click",(function(){n.qtip("hide")}))}}})}));var DesktopReporting=function(){var e=".form-group [id^=drp]",t=".js-save-info",n=".error",a={getExpiry:function(){var e=new Date;return new Date(e.setDate(e.getDate()+90))},errorClass:"invalid",errorNode:"<span class='error'>This field is required.</span>",cookiePath:"/",cookieDomain:location.hostname},o=function(){var t=!0;return jQuery(e).each((function(e,n){var o=jQuery(n);o.val()&&0!=o.val().trim().length&&!new RegExp("^[-]+$").test(o.val().trim())||(o.addClass(a.errorClass),o.before(a.errorNode),t=!1)})),t},i=function(){jQuery(e).each((function(e,t){var n=jQuery(this);jQuery.cookie(this.id,n.val(),{expires:a.getExpiry(),path:a.cookiePath,domain:a.cookieDomain})}))};return{init:function(){jQuery(t).on("click",(function(t){if(t.preventDefault(),jQuery(e).prev(n).remove(),o()){if(i(),jQuery.urlParam("pdfRequest")){var a="/stamp/stamp.jsp?tp=&arnumber="+jQuery.urlParam("arnumber");Modal.closeAndRedirectToUrl(a)}else jQuery.urlParam("getPdfRequest")?(a="/stampPDF/getPDF.jsp?tp=&arnumber="+jQuery.urlParam("arnumber"),Modal.closeAndRedirectToUrl(a)):jQuery.urlParam("htmlRequest")?(a="/document/"+jQuery.urlParam("arnumber")+"/",Modal.closeAndRedirectToUrl(a)):jQuery.urlParam("getIcpRequest")?(a="/xpls/icp.jsp?arnumber="+jQuery.urlParam("arnumber"),Modal.closeAndRedirectToUrl(a)):jQuery.urlParam("bookPdfRequest")&&(a="/xpl/ebooks/bookPdfWithBanner.jsp?fileName="+jQuery.urlParam("fileName")+"&bkn="+jQuery.urlParam("bkn")+"&pdfType="+jQuery.urlParam("pdfType"),Modal.closeAndRedirectToUrl(a));Modal.hide()}return!1})),jQuery(e).on("focus",(function(){jQuery(this).removeClass(a.errorClass).prev(n).remove()}))}}}();function deletePopularTerms(e,t){j$.post(e,{},jasonCallBack,"json")}function jasonCallBack(e){j$("#actionMessage").html('<p class="validation-failed validation-advice" style="padding: 0.5em; margin-top: 1em;">'+e.Message+"</p>"),location.href=location.href}j$((function(){j$(".JumpLink a").click((function(e){e.preventDefault();var t="#Body",n=window.location.pathname;n.indexOf("/xpl/articleDetails.jsp")>-1?t="#article-page-hdr":n.indexOf("/document/")>-1?t="#full-text-section":n.indexOf("home.jsp")>-1?t="#homePageTabs":(n.indexOf("/search/searchresult.jsp")>-1||n.indexOf("/browse/")>-1||n.indexOf("/virtual-journals/")>-1||n.indexOf("/courses/")>-1)&&(t="#xplMainContent");var a=j$(t);j$("html, body").animate({scrollTop:a.offset().top},750,(function(){a.focus().attr("tabindex","-1")}))}))})),function(){function e(e,t){var n=t.find("img"),a=t.closest(".controls").next(".abstract");handler={show:function(){n.attr("src",ASSETS_RELATIVE_PATH+"/img/btn.quick-abstract.expanded.gif?cv="+ASSETS_VERSION),n.attr("alt","Click to collapse"),a.slideDown()},hide:function(){n.attr("src",ASSETS_RELATIVE_PATH+"/img/btn.quick-abstract.collapsed.gif?cv="+ASSETS_VERSION),n.attr("alt","Click to expand"),a.slideUp()}},handler[e]()}jQuery("#results-blk .quick-ab-button").on("click",(function(){var t=jQuery(this);return"Click to expand"===t.find("img").attr("alt")?e("show",t):e("hide",t),!1}))}();