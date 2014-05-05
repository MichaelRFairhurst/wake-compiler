hljs.LANGUAGES.wake = {
  case_insensitive: true, // language is case-insensitive
  k: {
    keyword: 'every if while for is a an capable with public else switch case continue default break provides needs return this',
    type: 'Text Int Truth Printer'
  },

  c: [
    {
      cN: 'decorator',
      b: ':[a-zA-Z]+'
    },
    {
      cN: 'decorator',
      b: '@[a-zA-Z]+'
    },
    {
      cN: 'decorator',
      b: '\\$+'
    },
    {
      cN: 'string',
      b: '"', e: '"'
    },
    {
      cN: 'string',
      b: "'", e: "'"
    },
    {
      cN: 'number',
      b: '[0-9]+'
    },
    {
      cN: 'bold',
      b: ':'
    },
    {
      cN: 'comment',
      b: '#>', e: '##',
    },
    {
      cN: 'comment',
      b: '//', e: '$',
    }
  ]
}
hljs.initHighlightingOnLoad();

$(function() {
	setTimeout(function() {$('.goodcode, .badcode').popover({trigger: 'hover', container: 'body', placement: 'top'}); },10);

	var menuopen = false;
	var mobile = false;
	function showhideMobileMenu() {
		$('.content').css({
			paddingTop: '30px',
			left: '0px'
		});
		$('.mynavbar').css({
			width: '75%',
			top: '25px',
			display: menuopen? 'block' : 'none'
		});
		$('.opennav').show();
	}
	var resize = function() {
		if($(window).width() < 900) {
			mobile = true;
			showhideMobileMenu();
		} else {
			mobile = false;
			$('.content').css({
				left: '270px',
				paddingTop: '5px'
			});
			$('.mynavbar').css({
				display: 'block',
				width: '240px',
				top: '0px',
			});
			$('.opennav').hide();
		}
	};

	resize();

	$(window).resize(resize);

	$('.opennav button, .mynavbar .nav li').click(function() {
		if(!mobile) return;
		menuopen = !menuopen;
		showhideMobileMenu();
	});
});
