#!/usr/bin/tcsh
foreach set ( "001" "206" "004" )
    foreach rnk ( 0 1 )
        foreach corr ( 0 1 )
            foreach gt4GeV ( 0 1 )
                echo "./sums.csh nch_TProfile2D $set -1 $rnk $corr $gt4GeV"
                ./sums.csh nch_TProfile2D $set -1 $rnk $corr $gt4GeV
            end
        end
    end
end
        
