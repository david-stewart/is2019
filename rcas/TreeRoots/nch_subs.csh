#!/usr/bin/tcsh
#short version
foreach set ( "004" )
    foreach pos_ranking ( "0" "1" )
        foreach bbc_corr ( "0" "1" )
            sums.csh nch_TProfile2D ${set} 1000000 ${pos_ranking} ${bbc_corr} _short
        end
    end
end

#full version
foreach set ( "001" "004" "206")
    foreach pos_ranking ( "0" "1" )
        foreach bbc_corr ( "0" "1" )
            sums.csh nch_TProfile2D ${set} -1 ${pos_ranking} ${bbc_corr}
        end
    end
end
