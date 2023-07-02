%macro colon 2 ; %1 - key, %2 - value
    %ifstr %1
        %ifid %2
            %2:
                %ifdef first
                    dq first ; set new first element of dictionary
                %else
                    dq 0 ; define first element in dictionary 
                %endif
                db %1, 0 ; define new pair of dictionary key and value
                %define first %2 ; update first element of dictionary
        %else
            %error "second argument should be an id"
        %endif
    %else
        %error "first argument should be a string"
    %endif    
%endmacro