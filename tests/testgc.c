// implementare recursiva pentru factorial
int fact(int n){
	if(n<3)return n;
	return n*fact(n-1);
	}

void main(){
	put_i(5.913);		// se afiseaza 
	
	put_i(fact(6));	// se afiseaza 

	// implementare nerecursiva pentru factorial
	int r;
	r=1;
	int i;
	i=2;
	while(i<5){
		r=r*i;
		i=i+1;
		}
	put_i(r);		// se afiseaza 
	}
