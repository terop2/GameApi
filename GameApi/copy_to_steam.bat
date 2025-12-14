cd steam_tmp
scp terop@meshpage.org:/home/terop/GameApiBuilder_rel.zip .
c:\Windows\System32\tar xf GameApiBuilder_rel.zip
c:\Windows\System32\xcopy "c:\users\terop\cvs\GameApi\GameApi\steam_tmp\GameApiBuilder\*.*" "c:\users\terop\cvs\steam\sdk\tools\ContentBuilder\content\GameApiBuilder\depot_linux\" /E /I /H /Y
