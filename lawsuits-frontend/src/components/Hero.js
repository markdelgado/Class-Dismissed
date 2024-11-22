import React from "react";
import '../Hero.css'
import Logo from '../images/logoClassDismissed.png'


const Hero = () => {
    return(
        <div className="hero">   
            <img src={Logo} alt="logo" width="30%" content="center" padding-left="15%"/>
            <p>Discover and stay informed about the latest class action lawsuits. From settlement details to claim deadlines, we’ve got you covered. Empower yourself with the information you need to claim your share and protect your rights—all in one place.</p>
    </div >
    )
}

export default Hero;