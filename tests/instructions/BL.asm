org #2000

Start

; Test the BL instruction:

bl Finish	; Expected: Pre-branched PC is stored in LR. New PC -> 2400

org #2400

Finish

end	Start
