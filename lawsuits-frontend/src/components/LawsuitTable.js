import React, { useEffect, useState} from "react";
import { Table } from "react-bootstrap";

const LawsuitTable = () =>{
   const [lawsuits, setLawsuits] = useState([]);
   
    useEffect(() => {
        fetch('/lawsuits.json') // Adjusted path to go back two directories
            .then(response => {
                if (!response.ok) {
                    throw new Error(`HTTP error! Status: ${response.status}`);
                }
                return response.json();
            })
            .then(data => setLawsuits(data))
            .catch(error => console.error("Error fetching data:", error));
    }, []);
   
   
    return(
        <Table striped border hover>
            <thead>
                <tr>
                    <th>Title</th>
                    <th>Defandant</th>
                    <th>Details</th>
                    <th>Deadline</th>
                </tr>
            </thead>
            <tbody>
                {lawsuits.map((lawsuit, index)=>(
                    <tr key={index}>
                        <td>{lawsuit.title}</td>                        
                        <td>{lawsuit.defendant}</td> 
                        <td>{lawsuit.details}</td> 
                        <td>{lawsuit.deadline}</td> 
                    </tr>
                ))}
            </tbody>

        </Table>
    );
}

export default LawsuitTable;