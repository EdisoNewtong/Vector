------------------------------------------------------------------------------------------------------------------------------------------------------
Add a feature to trace the tag <Cursor> info ( For Qt Use )
------------------------------------------------------------------------------------------------------------------------------------------------------
			  a602f21  Edison Zhang Sun Jan 1 21:15:39 2023  +0800 EdisonLog : 1. fix PCDATA's first character couting issue ( if the first character is a multi-bytes character , the Qt Cursor is incorrect ) 2. fix a comment's bug (if skip node_comment , I miss a character the leader ch of "-->"
              ab54e5e  Edison Zhang Sat Dec 31 15:46:29 2022 +0800 EdisonLog : fix some bugs during parsing the type of  DOCTYPE
              bfcdc99  Edison Zhang Fri Dec 30 16:47:45 2022 +0800 EdisonLog : fix escaple sequence counting bug
              a1aeab3  Edison Zhang Thu Dec 29 22:49:18 2022 +0800 EdisonLog : fix a bug when ReParsing the attribute-list on the node whose type is node_declaration
              4eb116d  Edison Zhang Thu Dec 29 21:24:04 2022 +0800 EdisonLog : partail test [Passed]    for   Element / PC-Data / CData / Comment / Process Instruction / Declaraction
              9953922  Edison Zhang Sat Dec 17 22:25:37 2022 +0800 EdisonLog : parted done for    node_element && PCDATA
tag : V1.13   a0e0643  GitHub Fri Oct 28 16:53:25 2022 -0700 Merge pull request #526 from zeux/newver

