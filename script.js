
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.7/firebase-app.js";

// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
const firebaseConfig = {
    apiKey: "XXXXXXXXXXXXXXXXXXXX",
    authDomain: "XXXXXXXXXXXXXXXXXXXX",
    databaseURL: "XXXXXXXXXXXXXXXXXXXX",
    projectId: "XXXXXXXXXXXXXXXXXXXX",
    storageBucket: "XXXXXXXXXXXXXXXXXXXX",
    messagingSenderId: "XXXXXXXXXXXXXXXXXXXX",
    appId: "XXXXXXXXXXXXXXXXXXXX"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

import { getDatabase, ref, set, onValue, child, update, remove }
    from "https://www.gstatic.com/firebasejs/9.6.7/firebase-database.js";
const db = getDatabase();




$(document).ready(function () {
    function actual() {
        let fecha=new Date(); //Actualizar fecha.
        let hora=fecha.getHours(); //hora actual
        let minuto=fecha.getMinutes(); //minuto actual
        let segundo=fecha.getSeconds(); //segundo actual
        if (hora<10) { //dos cifras para la hora
            hora="0"+hora;
            }
        if (minuto<10) { //dos cifras para el minuto
            minuto="0"+minuto;
            }
        if (segundo<10) { //dos cifras para el segundo
            segundo="0"+segundo;
            }
        //devolver los datos:
        let mireloj = hora+" : "+minuto+" : "+segundo;	
        return mireloj; 
    }
    function actualizar() { //función del temporizador
        let mihora=actual(); //recoger hora actual        
        document.getElementById("text0").innerHTML=mihora; //incluir hora en elemento
    }
    setInterval(actualizar,1000); //iniciar temporizador

    var RiegoManual;
    var OnOff;
    var RiegoCada;
    var RiegoDurante;
    var quantum1=5;    
    var HoradeRiego;
    

    onValue(ref(db,), function (snap) {
        RiegoManual = snap.val().RiegoManual;
        OnOff = snap.val().OnOff;
        RiegoCada = snap.val().RiegoCada;
                
            
        RiegoDurante = snap.val().RiegoDurante;
       

        
        HoradeRiego = snap.val().HoradeRiego;

///////////////////////////////////////////////
        if (OnOff == "1") {
            document.getElementById("text8").innerHTML="El Riego esta Activado";
            document.getElementById("b8").style.background=" #f0d7e6";
            document.getElementById("b8").style.boxShadow=" 0 0 10px #f0d7e6, 0 0 40px #f0d7e6, 0 0 80px #f0d7e6;";
            document.getElementById("b8").style.borderRadius="90%";

            
        } else {
            document.getElementById("text8").innerHTML="El Riego esta Desactivado";
            document.getElementById("b8").style.background="";
            document.getElementById("b8").style.boxShadow="";
            document.getElementById("b8").style.borderRadius="";
            
        }

        

        document.getElementById("text3").innerHTML=(RiegoCada)+"HS";
        if(RiegoDurante>55){
            quantum1=60;
            document.getElementById("text2").innerHTML=((RiegoDurante)/60)+" Minutos";
        }

        else{
            quantum1=5;
            document.getElementById("text2").innerHTML=(RiegoDurante)+" Segundos";
        }

        document.getElementById("text1").innerHTML=(HoradeRiego)+"HS";
        

        
           
        
    
    })

    const b1 = document.querySelector("#b1")
    const b2 = document.querySelector("#b2")
    const b3 = document.querySelector("#b3")
    const b4 = document.querySelector("#b4")
    const b5 = document.querySelector("#b5")
    const b6 = document.querySelector("#b6")
    const b7 = document.querySelector("#b7")
    const b8 = document.querySelector("#b8")


    b1.addEventListener("click", e => {
        set(ref(db, 'RiegoManual'), true);
        
    })
///////////////////////////////////////////////////////////
    b2.addEventListener("click", e => {
        set(ref(db, 'HoradeRiego'), HoradeRiego+1 );
        HoradeRiego=Math.min(Math.max(HoradeRiego, 1), 23)
        
    })
    b3.addEventListener("click", e => {
        set(ref(db, 'HoradeRiego'), HoradeRiego-1 );
        HoradeRiego=Math.min(Math.max(HoradeRiego, 1), 23)
        
    })
/////////////////////////////////////////////////////////
    b4.addEventListener("click", e => {
        
        
        set(ref(db, 'RiegoDurante'), (RiegoDurante+quantum1),);
        RiegoDurante=Math.min(Math.max(RiegoDurante, 10), 3540)
        

    })
    b5.addEventListener("click", e => {
        
        set(ref(db, 'RiegoDurante'), (RiegoDurante-quantum1),);
        RiegoDurante=Math.min(Math.max(RiegoDurante, 10), 3540)
        if (RiegoDurante==0) {
            RiegoDurante==5;
        }
        
    })
/////////////////////////////////////////////////////////////
    b6.addEventListener("click", e => {
        set(ref(db, 'RiegoCada'), (RiegoCada+1),);
        RiegoCada=Math.min(Math.max(RiegoCada, 2), 95)
    })
    b7.addEventListener("click", e => {
        set(ref(db, 'RiegoCada'), (RiegoCada-1),);
        RiegoCada=Math.min(Math.max(RiegoCada, 2), 96)
        
    })
    b8.addEventListener("click", e => {
        set(ref(db, 'OnOff'), (!OnOff),);
    })

   
    

})



