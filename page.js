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

function moveAlerts() {
	var margin = $(window).width() - 240 - 1100;
	if(margin < 400) {
		$('.alert:not([immovable])').each(function() {
			$(this).css({position: 'relative', 'width': 'inherit', borderRadius: '4px', top: 'inherit'});
		});
	} else {
		var left = true;
		var offsetbase = $('.inner-content').offset().top;
		var minheightleft = offsetbase;
		var minheightright = offsetbase;

		$('.alert:not([immovable])').each(function() {
			$(this).css({position: 'absolute', width: margin / 2 - 100});

			if(left) {
				$(this).css({left: 10, borderRadius: '15px 0px 15px 15px'});
				if($(this).offset().top < minheightleft) $(this).css({top: minheightleft - offsetbase});
				minheightleft = $(this).offset().top + $(this).height() + 40;
			} else {
				$(this).css({right: 10, borderRadius: '0px 15px 15px 15px'});
				if($(this).offset().top < minheightright) $(this).css({top: minheightright - offsetbase});
				minheightright = $(this).offset().top + $(this).height() + 40;
			}

			left = !left;
		});
	}
}

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

		moveAlerts();
	};

	resize();

	$(window).resize(resize);

	$('.opennav button, .mynavbar .nav li').click(function() {
		if(!mobile) return;
		menuopen = !menuopen;
		showhideMobileMenu();
	});
});

